
#include "pusher.h"
#include "context.h"
#include "particle.h"

void addPusher(Context* context, Vector2 pos, Vector2 hitbox) {
    GameData* game = &context->gameData;

    Pusher pusher = {
        .pos = pos,
        .hitbox = hitbox,
        .angle = 30.0f,
        .distance = 100.0f,
        .radius = 200.0f,
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
        .force = (Vector2){300, 0},
        .particleCreateInfo = particleCreateInfo,
    };

    createParticleEmitter(context, &emitterCreateInfo);
}

void renderPusher(struct Context *context, Pusher *pusher) {
    DrawRectangle(pusher->pos.x, pusher->pos.y, pusher->hitbox.x, pusher->hitbox.y, WHITE);
}

