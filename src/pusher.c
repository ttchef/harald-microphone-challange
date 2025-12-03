
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
    float randomAngle = RandomFloat(pusher->offsetAngle - pusher->coneAngle, pusher->offsetAngle + pusher->coneAngle) * DEG2RAD;
    Vector2 dir = (Vector2){cosf(randomAngle), sinf(randomAngle)};
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
        .coneAngle = coneAngle,
        .distance = distance,
    };
    float angle = (pusher.offsetAngle + pusher.coneAngle) * DEG2RAD;
    pusher.pushVel = (Vector2){cosf(angle), sinf(angle)};
    darrayPush(game->pushers, pusher);

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
        Vector2 endPos1 = Vector2Add(Vector2Rotate((Vector2){pusher->distance, 0}, pusher->offsetAngle * DEG2RAD), pusher->pos);
        Vector2 endPos2 = Vector2Add(Vector2Rotate((Vector2){pusher->distance, 0}, (pusher->offsetAngle + pusher->coneAngle * 2) * DEG2RAD), pusher->pos);
        DrawLine(pusher->pos.x + pusher->hitbox.x / 2, pusher->pos.y + pusher->hitbox.y / 2, endPos1.x, endPos1.y, GREEN);
        DrawLine(pusher->pos.x + pusher->hitbox.x / 2, pusher->pos.y + pusher->hitbox.y / 2, endPos2.x, endPos2.y, GREEN);
        DrawLineEx(endPos1, endPos2, 1, GREEN);
    }
}

void renderPushers(struct Context *context) {
    Pusher* pushers = context->gameData.pushers;
    for (int32_t i = 0; i < darrayLength(pushers); i++) {
        Pusher* pusher = &pushers[i];
        DrawRectangle(pusher->pos.x, pusher->pos.y, pusher->hitbox.x, pusher->hitbox.y, WHITE);
    }
}
