
#ifndef PARTICLE_H
#define PARTICLE_H

#include "darray.h"
#include <raylib.h>

typedef struct ParticleCreateInfo {
    Vector2 dim;
    float lifetime;
} ParticleCreateInfo;

typedef struct Particle {
    bool isActive;
    Vector2 pos;
    Vector2 dim;
    float lifetime;
} Particle;

typedef struct EmitterCreateInfo {
    Vector2 pos;
    float lifetime;
    float spawnRate;
    ParticleCreateInfo particleCreateInfo;
} EmitterCreateInfo;

typedef struct Emitter {
    Vector2 pos;
    float lifetime;
    float spawnRate;
    ParticleCreateInfo particleCreateInfo;
    Particle* particles;
} Emitter;

typedef struct ParticleSystem {
    Emitter* emitters;
} ParticleSystem;

struct Context;
void initParticleSystem(struct Context* context);
void createParticleEmitter(struct Context* context, EmitterCreateInfo* emitterCreateInfo);

void updateParticleSystem(struct Context* context);
void drawParticleSystem(struct Context* context);

#endif
