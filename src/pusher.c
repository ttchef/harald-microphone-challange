
#include "pusher.h"
#include "context.h"
#include "darray.h"
#include "particle.h"
#include "utils.h"
#include <raylib.h>

Particle particleCallback(struct Context* context, struct Emitter* emitter, ParticleCreateInfo* particleCreateInfo) {
    uint32_t index = (uint32_t)(uintptr_t)emitter->userData;
    Pusher* pusher = &context->gameData.pushers[index];

    Particle p = {
        .dim = particleCreateInfo->dim,
        .lifetime = particleCreateInfo->lifetime,
        .color = particleCreateInfo->color,
        .vel = emitter->force,
    };

    // Dircetion Vector
    float randomAngle = (RandomFloat(0, pusher->coneAngle * 2) + pusher->offsetAngle) * DEG2RAD;
    Vector2 dir = (Vector2){cosf(randomAngle), -sinf(randomAngle)};
    dir = Vector2Scale(dir, pusher->distance * 0.5f);

    Vector2 vel = (Vector2){0};
    //vel.x = GetRandomValue(-10, 10);
    //vel.y = GetRandomValue(-10, 10);
    p.vel = Vector2Add(dir, vel);

    p.pos = emitter->pos;
    return p;
}

void addPusher(Context* context, Vector2 pos, Vector2 hitbox, float coneAngle,
               float offsetAngle, float distance) {
    GameData* game = &context->gameData;

    Pusher pusher = {
        .pos = pos,
        .hitbox = hitbox,
        .offsetAngle = offsetAngle,
        .coneAngle = coneAngle / 2,
        .distance = distance,
    };
    float angle = (pusher.offsetAngle + pusher.coneAngle) * DEG2RAD;
    pusher.pushVel = (Vector2){cosf(angle), sinf(angle)};
    darrayPush(game->pushers, pusher);

    ParticleCreateInfo particleCreateInfo = {
        .lifetime = 2.0f,
        .dim = (Vector2){10.0f, 10.0f},
        .color = RED,
    };

    EmitterCreateInfo emitterCreateInfo = {
        .pos =  pos,
        .lifetime = 10.0f,
        .infinite = true,
        .spawnRate = 30.0f,
        .force = (Vector2){0, 0},
        .particleCreateInfo = particleCreateInfo,
        .spawnCallback = particleCallback,
        .userData = (void*)(uintptr_t)(darrayLength(game->pushers) - 1),
    };

    createParticleEmitter(context, &emitterCreateInfo);
}

void debugRenderPushersHitbox(struct Context *context) {
    Pusher* pushers = context->gameData.pushers;
    for (int32_t i = 0; i < darrayLength(pushers); i++) {
        Pusher* pusher = &pushers[i];
        
        // Block
        DrawRectangleLines(pusher->pos.x, pusher->pos.y, pusher->hitbox.x, pusher->hitbox.y, GREEN);

        // Cone
        Vector2 pushCenter = (Vector2){pusher->pos.x + pusher->hitbox.x / 2, pusher->pos.y + pusher->hitbox.y / 2};
        float oaRad = pusher->offsetAngle * DEG2RAD;
        float caRad = pushers->coneAngle * DEG2RAD;
        Vector2 endPos1 = Vector2Add(pushCenter, Vector2Scale((Vector2){cosf(oaRad), -sinf(oaRad)}, pusher->distance));
        Vector2 endPos2 = Vector2Add(pushCenter, Vector2Scale((Vector2){cosf(oaRad + caRad * 2), -sinf(oaRad + caRad * 2)}, pusher->distance));
        DrawLineEx(pushCenter, endPos1, 3, RED);
        DrawLineEx(pushCenter, endPos2, 3, RED);
        DrawLineEx(endPos1, endPos2, 3, RED);
    }
}

void renderPushers(struct Context *context) {
    Pusher* pushers = context->gameData.pushers;
    for (int32_t i = 0; i < darrayLength(pushers); i++) {
        Pusher* pusher = &pushers[i];
        DrawRectangle(pusher->pos.x, pusher->pos.y, pusher->hitbox.x, pusher->hitbox.y, WHITE);
    }
}
