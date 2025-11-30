
#ifndef UTILS_H
#define UTILS_H

#include <raylib.h>

static inline float RandomFloat(float min, float max) {
    return min + (float)GetRandomValue(0, 10000) / 10000.0f * (max - min);
}

#endif // UTILS_H
