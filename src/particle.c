
#include "particle.h"
#include "context.h"
#include "darray.h"

#include <stdio.h>

void initParticleSystem(struct Context *context) {
    ParticleSystem* pr = &context->gameData.particleSystem;
    pr->emitters = darrayCreate(Emitter);
}

void createParticleEmitter(struct Context* context, EmitterCreateInfo* emitterCreateInfo) {
    ParticleSystem* pr = &context->gameData.particleSystem;
    EmitterCreateInfo cr = *emitterCreateInfo;

    Emitter emitter = {
        .pos = cr.pos,
        .lifetime = cr.lifetime,
        .spawnRate = cr.spawnRate,
        .force = cr.force,
        .infinite = cr.infinite,
        .particleTexture = cr.particleTexture == NULL ? (Texture2D){0} : LoadTexture(cr.particleTexture),
        .hasTexture = cr.particleTexture == NULL ? false : true,
        .particleCreateInfo = cr.particleCreateInfo,
    };

    emitter.particleSpawnTime = 1 / cr.spawnRate;
    emitter.particles = darrayCreate(Particle);
    darrayPush(pr->emitters, emitter);
}

Particle createRandomParticle(Emitter* emitter, ParticleCreateInfo* particleCreateInfo) {
    Particle p = {
        .dim = particleCreateInfo->dim,
        .lifetime = particleCreateInfo->lifetime,
        .color = particleCreateInfo->color,
    };

    Vector2 pos;
    pos.x = GetRandomValue(emitter->pos.x + particleCreateInfo->dim.x / 2 - 30,
                           emitter->pos.x + particleCreateInfo->dim.x / 2 + 30);
    pos.y = GetRandomValue(emitter->pos.y + particleCreateInfo->dim.y / 2 - 30,
                           emitter->pos.y + particleCreateInfo->dim.y + 30);
    p.pos = pos;

    return p;
}

void updateParticleSystem(struct Context *context) {
    ParticleSystem* pr = &context->gameData.particleSystem;
    for (int32_t i = 0; i < darrayLength(pr->emitters); i++) {
        Emitter* emitter = &pr->emitters[i];
        emitter->elapsedTime += context->deltaTime;
 
        // Check if still active
        if (!emitter->infinite && emitter->lifetime <= 0.0f) {
            darrayPopAt(pr->emitters, i, NULL);
        }
        if (!emitter->infinite) emitter->lifetime -= context->deltaTime;

        if (emitter->elapsedTime >= emitter->particleSpawnTime) {
            Particle tmp = createRandomParticle(emitter, &emitter->particleCreateInfo);
            darrayPush(emitter->particles, tmp);
            emitter->elapsedTime = 0.0f;
        }

        // Update Particles
        for (int32_t j = 0; j < darrayLength(emitter->particles); j++) {
            Particle* p = &emitter->particles[j];

            p->pos.x += emitter->force.x * context->deltaTime;
            p->pos.y += emitter->force.y * context->deltaTime;

            if (p->lifetime <= 0.0f) {
                emitter->particles = darrayPopAt(emitter->particles, j, NULL);
            }
            p->lifetime -= context->deltaTime;
     
        }
    }
}

void drawParticleSystem(struct Context *context) {
    ParticleSystem* pr = &context->gameData.particleSystem;
    for (int32_t i = 0; i < darrayLength(pr->emitters); i++) {
        Emitter* emitter = &pr->emitters[i];
 
        for (int32_t j = 0; j < darrayLength(emitter->particles); j++) {
            Particle* p = &emitter->particles[j];
            Rectangle src = {
                .x = 0,
                .y = 0,
                .width = emitter->particleTexture.width,
                .height = emitter->particleTexture.height,
            };
            Rectangle dest = {
                .x = p->pos.x,
                .y = p->pos.y,
                .width = p->dim.x,
                .height = p->dim.y
            };
            Color c = (Color){255, 255, 255, p->lifetime * 255};
            
            if (emitter->hasTexture) {
                DrawTexturePro(emitter->particleTexture, src, dest, (Vector2){0, 0}, 0.0f, c);
            }
            else {
                DrawRectangle(p->pos.x, p->pos.y, p->dim.x, p->dim.y, c);
            }
        }
    }
}

