
#include <game.h>
#include <context.h>
#include <raymath.h>

#include <string.h>
#include <stdatomic.h>

void initGame(Context *context) {
    GameData* data = &context->gameData;
    data->groundY = context->windowHeight * 0.9f;

    // Player
    data->player.pos = (Vector2){context->windowWidth * 0.2f, context->windowHeight * 0.5f};
    data->player.dim = (Vector2){40, 40};
}

static void updatePlayer(Context *context, float dt) {
    GameData* gameData = &context->gameData;
    Player* p = &gameData->player;
    
    // Over threshold?
    float pitch = getPitch(context);
    p->pitchFiltered = 0.9f * p->pitchFiltered + 0.1f * pitch;
    pitch = p->pitchFiltered;
    int32_t volume = fmaxf(atomic_load(&context->data.volR), atomic_load(&context->data.volL));
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

void updateGame(Context *context, float dt) {
    updatePlayer(context, dt);
    printf("Picth: %f\n", context->gameData.player.pitchFiltered);
}


