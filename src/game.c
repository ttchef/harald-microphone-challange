
#include "background.h"
#include "collider.h"
#include "particle.h"
#include <game.h>
#include <context.h>
#include <audio.h>
#include <raylib.h>
#include <raymath.h>

#include <stdint.h>
#include <string.h>
#include <stdatomic.h>

#define PLAYER_SPRITESHEET_GRIDSIZE 64
#define PLAYER_SPRITESHEET_COLUM_COUNT 6
#define PLAYER_SPRITESHEET_ROW_COUNT 6

typedef struct PlayerSpriteAnimationInfo {
    Vector2 startPos;
    int32_t length;
} PlayerSpriteAnimationInfo;

const PlayerSpriteAnimationInfo playerSpriteAnimationInfo[PLAYER_ANIM_STATE_NUM] = {
    {{0, 0,}, 6},
    {{0, 1}, 9},
    {{3, 2}, 10},
    {{1, 4}, 3},
    {{4, 4}, 8},
};

static void initPlatforms(Context* context) {
    GameData* game = &context->gameData;

    float minX = 50;
    float maxX = context->gameWidth - 300;

    int32_t currentY = game->groundY;

    for (int32_t i = 0; i < GAME_MAX_OBSTACLES; i++) {
        Platform* p = &game->platforms[i];
        Collider* c = &p->collider;
        p->isActive = true;
        c->type = COLLIDER_TYPE_ONE_WAY_PLATFORM;

        if (i == 0) {
            c->pos.x = game->player.pos.x;
        } 
        else {
            int shift = GetRandomValue(80, 240);

            int neg = (GetRandomValue(0, 100) < 50) ? 1 : -1;

            float newX = game->platforms[i - 1].collider.pos.x + shift * neg;

            c->pos.x = Clamp(newX, minX, maxX);
        }

        currentY -= GetRandomValue(90, 150);
        c->pos.y = currentY;

        // Pusher 
        if (i % 3 == 0) {
            addPusher(context, (Vector2){c->pos.x, c->pos.y - 20}, (Vector2){20, 20},
                      30.0f, -30.0f, 200.0f);
        }

        // Textures
        p->left = LoadTexture("res/erik/platform/left.png");
        p->right = LoadTexture("res/erik/platform/right.png");
        p->middle = LoadTexture("res/erik/platform/middle.png");

        c->dim.x = (p->left.width + p->middle.width + p->right.width) * PLATFORM_SCALE;
        c->dim.y = p->middle.height * PLATFORM_SCALE;
    }
}

void initGame(Context *context) {
    GameData* data = &context->gameData;
    initBackground(context, &data->bg);
    data->groundY = context->windowHeight * 0.9f;
    data->groundTex = LoadTexture("res/erik/ground.png");

    // Player
    data->player.texture = LoadTexture("res/StickmanPack-V0.2/StickmanPack/Full/Full.png");
    data->player.framesSpeed = 8;
    data->player.frameRec = (Rectangle){0.0f, 0.0f,
        data->player.texture.width / (float)PLAYER_SPRITESHEET_COLUM_COUNT,
        data->player.texture.height / (float)PLAYER_SPRITESHEET_ROW_COUNT};
    data->player.accentColor = WHITE;
    data->player.pos = (Vector2){context->windowWidth * 0.2f, context->windowHeight * 0.5f};
    data->player.dim = (Vector2){PLAYER_SPRITESHEET_GRIDSIZE * PLAYER_SCALE, PLAYER_SPRITESHEET_GRIDSIZE * PLAYER_SCALE};
    data->player.hitbox = (Vector2){PLAYER_SPRITESHEET_GRIDSIZE / 3.0f * PLAYER_SCALE, PLAYER_SPRITESHEET_GRIDSIZE * 0.8f * PLAYER_SCALE};
    data->player.hitboxOrigin = (Vector2){ (PLAYER_SPRITESHEET_GRIDSIZE * PLAYER_SCALE) * 0.3f, PLAYER_SPRITESHEET_GRIDSIZE * PLAYER_SCALE * 0.15f};
    data->player.direction = PLAYER_DIRECTION_RIGHT;

    // Player 2
    memcpy(&data->player2, &data->player, sizeof(Player));
    data->player2.texture = LoadTexture("res/StickmanPack-V0.2/StickmanPack/Full/Full.png");
    data->player2.accentColor = PINK;

    initParticleSystem(context);

    ParticleCreateInfo particleCreateInfo = {
        .lifetime = 0.7f,
        .dim = (Vector2){10.0f, 10.0f},
        .color = RED,
    };

    EmitterCreateInfo emitterCreateInfo = {
        .pos =  Vector2Add(data->player.hitbox, Vector2Subtract(data->player.hitboxOrigin, data->player.pos)),
        .particleTexture = "res/circle.png",
        .lifetime = 10.0f,
        .infinite = true,
        .spawnRate = 30.0f,
        .force = (Vector2){0, -300.0f},
        .particleCreateInfo = particleCreateInfo,
    };
    createParticleEmitter(context, &emitterCreateInfo);

    createRope(&data->rope, 50, (Vector2){context->windowWidth * 0.5f, context->windowHeight * 0.5f}, 5, 0.98f);
    data->pushers = darrayCreate(Pusher);
    initPlatforms(context);

}

static void spawnRandomObstacle(Context* context) {
    GameData* gameData = &context->gameData;
    for (int32_t i = 0; i < GAME_MAX_OBSTACLES; i++) {
        if (gameData->obstacles[i].isActive) continue;
        
        Obstacle* obs = &gameData->obstacles[i];
        obs->isActive = true;
        obs->dim = (Vector2){20, 20};
        obs->pos.x = context->gameWidth * 0.8f;
        obs->pos.y = context->windowHeight * 0.8f;
        
        break;
    }
}

static void updateObstacles(Context* context, float dt) {
    GameData* gameData = &context->gameData;
    for (int32_t i = 0; i < GAME_MAX_OBSTACLES; i++) {
        if (!gameData->obstacles[i].isActive) continue;

        Obstacle* obs = &gameData->obstacles[i];
        obs->pos.x -= 150.0f * dt;

        // Delete
        if (obs->pos.x + obs->dim.x < 0) obs->isActive = false;
    }
}

static void applyPlayerRopeConstraing(Context* context) {
    GameData* game = &context->gameData;
    Player* p1 = &game->player;
    Player* p2 = &game->player2;
    Rope* rope = &game->rope;

    Vector2 delta = Vector2Subtract(p2->pos, p1->pos);
    float dist = Vector2Length(delta);

    if (dist > rope->ropeSize) {
        float stretch = dist - rope->ropeSize;
        Vector2 dir = Vector2Scale(delta, 1.0f / dist);

        float k = 8.0f;
        float forceMagnitude = stretch * k;

        if (forceMagnitude > 500.0f) forceMagnitude = 500.0f;

        Vector2 pull = Vector2Scale(dir, forceMagnitude);

        p1->vel = Vector2Add(p1->vel, pull);
        p2->vel = Vector2Subtract(p2->vel, pull);
    }
}

static void updatePlayer(Context *context, float dt, PlayerIdentifier playerId) {
    GameData* gameData = &context->gameData;
    Player* p = playerId == PLAYER_1 ? &gameData->player : &gameData->player2;
    p->oldPos = p->pos;

    // Update Texture
    p->frameCounter++;
    if (p->frameCounter >= (60 / p->framesSpeed) && !(p->animState == PLAYER_ANIM_STATE_JUMP && p->currentFrame == 2)) {
        p->frameCounter = 0;

        p->currentFrame++;
        if (p->currentFrame > playerSpriteAnimationInfo[p->animState].length - 1) p->currentFrame = 0;

        Vector2 startPos = playerSpriteAnimationInfo[p->animState].startPos;
        int32_t frameWidth = p->texture.width / PLAYER_SPRITESHEET_COLUM_COUNT;
        int32_t frameHeight = p->texture.height / PLAYER_SPRITESHEET_ROW_COUNT;

        int32_t absoluteIndex = startPos.x + p->currentFrame;

        int32_t frameX = (absoluteIndex % PLAYER_SPRITESHEET_COLUM_COUNT) * frameWidth;
        int32_t frameY = (startPos.y + (int32_t)(absoluteIndex / PLAYER_SPRITESHEET_COLUM_COUNT)) * frameHeight;

        p->frameRec = (Rectangle){frameX, frameY, frameWidth, frameHeight};
    }

    // Over threshold?
    float pitch = getPitch(context, playerId);
    //printf("Pitch: %f\n", pitch);
    p->pitchFiltered = 0.9f * p->pitchFiltered + 0.1f * pitch;
    pitch = p->pitchFiltered;

    int32_t volume;
    if (playerId == PLAYER_1) volume = fmaxf(atomic_load(&context->data.volR), atomic_load(&context->data.volL)) * context->volumeMul;
    else if (playerId == PLAYER_2) volume = fmaxf(atomic_load(&context->dataP2.volR), atomic_load(&context->dataP2.volL)) * context->volumeMulP2;
    if (volume > 400) volume = 400;

    if (volume > PLAYER_JUMP_THRESHOLD && p->onGround) {
        p->vel.y = -volume * PLAYER_VOLUME_VELO_FACTOR;
        p->isJumping = true;
        p->volPeak = volume;
        p->strengthCooldown = PLAYER_STRENGTH_COOLDOWN;
    }
    else if (p->isJumping) {
        if (p->strengthCooldown <= 0.0f) {
            p->isJumping = false;
        }
        if (volume > p->volPeak) {
            float diff = volume - p->volPeak;
            p->vel.y -= diff * PLAYER_VOLUME_VELO_FACTOR;
            p->volPeak = volume;
        }
        p->strengthCooldown -= dt;
    }

    if (volume > PLAYER_JUMP_THRESHOLD / 2 && pitch < 170) {
        p->vel.x += pitch / 4;
    }
    else if (volume > PLAYER_JUMP_THRESHOLD / 2 && pitch >= 170) {
        p->vel.x -= pitch / 4;
    }
    if (p->vel.x > 500.0f) p->vel.x = 500.0f;
    else if (p->vel.x < -500.0f) p->vel.x = -500.0f;

    p->acc.y = GAME_GRAVITY;
    if (p->vel.x < 0) p->vel.x += GROUND_FRICTION;
    else if (p->vel.x > 0) p->vel.x -= GROUND_FRICTION;

    p->vel = Vector2Add(p->vel, Vector2Scale(p->acc, dt));
    p->pos.x += p->vel.x * dt;

    p->onGround = false;

    // Check collider collisios
    for (int32_t i = 0; i < GAME_MAX_OBSTACLES; i++) {
        if (!gameData->platforms[i].isActive) continue;

        Collider* coll = &gameData->platforms[i].collider;
        if (coll->type == COLLIDER_TYPE_ONE_WAY_PLATFORM) continue;
        if (checkAABBPlayer(coll, p)) {
            if (p->vel.x > 0) 
                p->pos.x = coll->pos.x - p->hitbox.x - p->hitboxOrigin.x;
            else if (p->vel.x < 0) 
                p->pos.x = coll->pos.x + coll->dim.x - p->hitboxOrigin.x;
            p->vel.x = 0;
        }
    }
    p->pos.y += p->vel.y * dt;

    for (int32_t i = 0; i < GAME_MAX_OBSTACLES; i++) {
        if (!gameData->platforms[i].isActive) continue;
        
        Collider* coll = &gameData->platforms[i].collider;
        if (checkAABBPlayer(coll, p)) {
            if (p->vel.y > 0) { 
                p->pos.y = coll->pos.y - p->hitbox.y - p->hitboxOrigin.y;
                p->onGround = true;
            } else if (p->vel.y < 0) 
                p->pos.y = coll->pos.y + coll->dim.y - p->hitboxOrigin.y;

            p->vel.y = 0;
        }
    }

    // Ground Collision
    if (p->pos.y + p->hitboxOrigin.y + p->hitbox.y > gameData->groundY && p->vel.y > 0) {
        p->pos.y = gameData->groundY - p->hitboxOrigin.y - p->hitbox.y;
        p->onGround = true;
    }

    // Wall Collision
    // No accurate collision because there is no need
    if (p->pos.x < -context->gameWidth / 2 && p->vel.x <= 0) {
        p->pos.x = context->gameWidth * 2 - p->dim.x - 10;
        if (context->isMultiplayer) {
            Player* p2 = playerId == PLAYER_1 ? &gameData->player2 : &gameData->player;
            p2->pos = p->pos;
        }
    }
    else if (p->pos.x + p->dim.x > context->gameWidth * 2) {
        p->pos.x = -context->gameWidth / 2;
        if (context->isMultiplayer) {
            Player* p2 = playerId == PLAYER_1 ? &gameData->player2 : &gameData->player;
            p2->pos = p->pos;
        }
    }

    // Change player direction
    // Higher values so no glitching between flipping of character
    if (p->vel.x < -15.0f) {
        p->direction = PLAYER_DIRECTION_LEFT;
    } 
    else if (p->vel.x > 15.0f) {
        p->direction = PLAYER_DIRECTION_RIGHT;
    }

    // Change Animation State
    if (p->isJumping && (p->vel.y < -30.0f || p->vel.y > 30.0f)) p->animState = PLAYER_ANIM_STATE_JUMP;
    else if (p->onGround) {
        if (p->vel.x < 30.0f && p->vel.x > -30.0f) p->animState = PLAYER_ANIM_STATE_IDLE;
        else p->animState = PLAYER_ANIM_STATE_RUN;
    }
}

static void handleInput(Context* context) {
    if (IsKeyPressed(KEY_A)) {
        spawnRandomObstacle(context);
    }
    // For Testing :/
    if (IsKeyPressed(KEY_B)) {
        context->gameData.player.vel.y = -2000;
    }
    if (IsKeyPressed(KEY_C)) {
        context->gameData.player.animState++;
        if (context->gameData.player.animState >= PLAYER_ANIM_STATE_NUM) {
            context->gameData.player.animState = 0;
        }
    }
    if (IsKeyPressed(KEY_D)) {
        context->debugMode = !context->debugMode;
    }
}

void updateGame(Context *context, float dt) {
    GameData* game = &context->gameData;
    handleInput(context);
    updatePlayer(context, dt, PLAYER_1);
    updateObstacles(context, dt);
    updateBackround(context, &game->bg, dt);
    updateParticleSystem(context);
    game->particleSystem.emitters[0].pos = Vector2Subtract(game->player.hitbox, Vector2Subtract(game->player.hitboxOrigin, game->player.pos));
    game->particleSystem.emitters[0].pos.x += game->player.hitboxOrigin.x;


    if (context->isMultiplayer) {
        updatePlayer(context, dt, PLAYER_2);
        Vector2 centerP1 = (Vector2){game->player.pos.x + game->player.hitboxOrigin.x + game->player.hitbox.x / 2,
                                        game->player.pos.y + game->player.hitboxOrigin.y + game->player.hitbox.y / 2};

        Vector2 centerP2 = (Vector2){game->player2.pos.x + game->player2.hitboxOrigin.x + game->player2.hitbox.x / 2,
                                        game->player2.pos.y + game->player2.hitboxOrigin.y + game->player2.hitbox.y / 2};

        applyPlayerRopeConstraing(context);
        updateRope(&context->gameData.rope, dt, centerP1, centerP2);
    }

    // Camera
    context->camera.target = game->player.pos;
    context->camera.offset = (Vector2){context->gameWidth * 0.5f - game->player.dim.x / 2, context->windowHeight * 0.5f};

    // Sets height
    game->height = -(game->player.pos.y - game->groundY);
    if (context->isMultiplayer && -(game->player2.pos.y - game->groundY) > game->height) game->height = -(game->player2.pos.y - game->groundY);
    if (game->height > game->maxHeight) game->maxHeight = game->height;
}

void deinitGame(Context *context) {
    GameData* game = &context->gameData;
    deinitBackground(&game->bg);

    for (int32_t i = 0; i < GAME_MAX_OBSTACLES; i++) {
        UnloadTexture(game->platforms[i].left);
        UnloadTexture(game->platforms[i].right);
        UnloadTexture(game->platforms[i].middle);
    }

    UnloadTexture(game->player.texture);
    UnloadTexture(game->player2.texture);
    UnloadTexture(game->groundTex);
}

