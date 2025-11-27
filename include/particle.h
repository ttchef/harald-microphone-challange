
#ifndef PARTICLE_H
#define PARTICLE_H

#include <stdint.h>

#include <raylib.h>

typedef struct ParticleSystem {
    Emitter* emitters;
} ParticleSystem;

typedef struct Emitter {
    Vector2 pos;
    float time;
    Particle* particles;
} Emitter;

typedef struct Particle {
    Vector2 pos;
} Particle;

#endif
