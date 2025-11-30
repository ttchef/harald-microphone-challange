
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

    Vector2 vel;
    vel.x = GetRandomValue(-10, 10);
    vel.y = GetRandomValue(-10, 10);
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

void renderPusher(struct Context *context, Pusher *pusher) {
    DrawRectangle(pusher->pos.x, pusher->pos.y, pusher->hitbox.x, pusher->hitbox.y, WHITE);
}

