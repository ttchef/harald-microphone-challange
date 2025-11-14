
#ifndef CONTEXT_H
#define CONTEXT_H

#include "audio.h"

typedef struct Context {
    const int32_t windowWidth;
    const int32_t windowHeight;
    const int32_t gameWidth;
    const int32_t guiWidth;
    const int32_t guiOffset;
    AudioData data;
} Context;

#endif // CONTEXT_H
