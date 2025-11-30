
#ifndef PARTICLE_H
#define PARTICLE_H

#include "darray.h"
#include <raylib.h>

typedef struct ParticleCreateInfo {
    Vector2 dim;
    float lifetime;
    Color color;
} ParticleCreateInfo;

typedef struct Particle {
    Vector2 pos;
    Vector2 dim;
    float lifetime;
    Color color;
} Particle;

typedef struct EmitterCreateInfo {
    bool infinite;
    Vector2 pos;
    float lifetime;
    float spawnRate;
    Vector2 force;
    const char* particleTexture;
    ParticleCreateInfo particleCreateInfo;
} EmitterCreateInfo;

typedef struct Emitter {
    bool infinite;
    Vector2 pos;
    Vector2 force;
    float lifetime;
    float spawnRate;
    float elapsedTime;
    float particleSpawnTime;
    bool hasTexture;
    Texture2D particleTexture;
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
