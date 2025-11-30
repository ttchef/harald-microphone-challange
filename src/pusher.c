
#include "pusher.h"
#include "context.h"
#include "particle.h"
#include "utils.h"

void addPusher(Context* context, Vector2 pos, Vector2 hitbox) {
    GameData* game = &context->gameData;

    Pusher pusher = {
        .pos = pos,
        .hitbox = hitbox,
        .angle = 240.0f,
        .distance = 100.0f,
        .radius = 200.0f,
    };
    darrayPush(game->pushers, pusher);

    // Dircetion Vector
    float centerAngleDeg = 240.0f;
    float centerAngle = centerAngleDeg * DEG2RAD;
    float halfCone = (centerAngleDeg * DEG2RAD) * 0.5f;

    float angle = centerAngle + RandomFloat(-halfCone, halfCone);

    Vector2 dir = (Vector2){cosf(angle), sinf(angle)};
    dir = Vector2Scale(dir, 200.0f);

    ParticleCreateInfo particleCreateInfo = {
        .lifetime = 0.7f,
        .dim = (Vector2){10.0f, 10.0f},
        .color = RED,
    };

    EmitterCreateInfo emitterCreateInfo = {
        .pos =  pos,
        .lifetime = 10.0f,
        .infinite = true,
        .spawnRate = 30.0f,
        .force = dir,
        .particleCreateInfo = particleCreateInfo,
    };

    createParticleEmitter(context, &emitterCreateInfo);
}

void renderPusher(struct Context *context, Pusher *pusher) {
    DrawRectangle(pusher->pos.x, pusher->pos.y, pusher->hitbox.x, pusher->hitbox.y, WHITE);
}

