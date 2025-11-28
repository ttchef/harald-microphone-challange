
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
        .particleCreateInfo = cr.particleCreateInfo,
    };

    emitter.particles = darrayCreate(Particle);
    darrayPush(pr->emitters, emitter);
}

Particle createRandomParticle(Emitter* emitter, ParticleCreateInfo* particleCreateInfo) {
    Particle p = {
        .dim = particleCreateInfo->dim,
        .lifetime = particleCreateInfo->lifetime,
    };

    Vector2 pos;
    pos.x = GetRandomValue(emitter->pos.x - 30, emitter->pos.x + 30);
    pos.y = GetRandomValue(emitter->pos.y - 30, emitter->pos.y + 30);
    p.pos = pos;

    return p;
}

void updateParticleSystem(struct Context *context) {
    ParticleSystem* pr = &context->gameData.particleSystem;
    for (int32_t i = 0; i < darrayLength(pr->emitters); i++) {
        Emitter* emitter = &pr->emitters[i];
 
        // Check if still active
        if (emitter->lifetime <= 0.0f) {
            darrayPopAt(pr->emitters, i, NULL);
        }
        emitter->lifetime -= context->deltaTime;

        uint64_t particleCount = darrayLength(emitter->particles);
        if (particleCount < emitter->particleCreateInfo.lifetime * emitter->spawnRate) {
            Particle tmp = createRandomParticle(emitter, &emitter->particleCreateInfo);
            darrayPush(emitter->particles, tmp);
        }

        // Update Particles
        for (int32_t j = 0; j < darrayLength(emitter->particles); j++) {
            Particle* p = &emitter->particles[j];

            p->pos.y -= 30.0f * context->deltaTime;

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
            DrawRectangle(p->pos.x, p->pos.y, p->dim.x, p->dim.y, WHITE);
        }
    }
}
