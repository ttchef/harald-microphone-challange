
#include <game.h>
#include <context.h>
#include <audio.h>
#include <raylib.h>
#include <raymath.h>

#include <string.h>
#include <stdatomic.h>

void initGame(Context *context) {
    GameData* data = &context->gameData;
    data->groundY = context->windowHeight * 0.9f;

    // Player
    data->player.pos = (Vector2){context->windowWidth * 0.2f, context->windowHeight * 0.5f};
    data->player.dim = (Vector2){40, 40};

    memcpy(&data->player2, &data->player, sizeof(Player));
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

static void updatePlayer(Context *context, float dt, PlayerIdentifier playerId) {
    GameData* gameData = &context->gameData;
    Player* p = playerId == PLAYER_1 ? &gameData->player : &gameData->player2;

    // Over threshold?
    float pitch = getPitch(context, playerId);
    printf("Pitch: %f\n", pitch);
    p->pitchFiltered = 0.9f * p->pitchFiltered + 0.1f * pitch;
    pitch = p->pitchFiltered;

    int32_t volume;
    if (playerId == PLAYER_1) volume = fmaxf(atomic_load(&context->data.volR), atomic_load(&context->data.volL));
    else if (playerId == PLAYER_2) volume = fmaxf(atomic_load(&context->dataP2.volR), atomic_load(&context->dataP2.volL));

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

    if (volume > PLAYER_JUMP_THRESHOLD / 2 && pitch < 250) {
        p->vel.x += pitch / 4;
    }
    else if (volume > PLAYER_JUMP_THRESHOLD / 2 && pitch >= 250) {
        p->vel.x -= pitch / 4;
    }
    if (p->vel.x > 500.0f) p->vel.x = 500.0f;
    else if (p->vel.x < -500.0f) p->vel.x = -500.0f;

    p->acc.y = GAME_GRAVITY;
    if (p->vel.x < 0) p->vel.x += GROUND_FRICTION;
    else if (p->vel.x > 0) p->vel.x -= GROUND_FRICTION;
    p->vel = Vector2Add(p->vel, Vector2Scale(p->acc, dt));
    p->pos = Vector2Add(p->pos, Vector2Scale(p->vel, dt));

    // Ground Collision
    p->onGround = false;
    if (p->pos.y + p->dim.y > gameData->groundY && p->vel.y > 0) {
        p->pos.y = gameData->groundY - p->dim.y;
        p->onGround = true;
    }

    // Wal Collision
    if (p->pos.x < 0 && p->vel.x <= 0) {
        p->pos.x = 0;
        p->vel.x *= -0.4f;
    }
    else if (p->pos.x + p->dim.x > context->gameWidth) {
        p->pos.x = context->gameWidth - p->dim.x;
        p->vel.x *= -0.4f;
    }
}

static void handleInput(Context* context) {
    if (IsKeyPressed(KEY_A)) {
        spawnRandomObstacle(context);
    }
}

void updateGame(Context *context, float dt) {
    handleInput(context);
    updatePlayer(context, dt, PLAYER_1);
    if (context->isMultiplayer) updatePlayer(context, dt, PLAYER_2);
    updateObstacles(context, dt);
    //printf("Picth: %f\n", context->gameData.player2.pitchFiltered);
}


