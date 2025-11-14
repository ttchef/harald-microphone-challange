
#ifndef CONTEXT_H
#define CONTEXT_H

#include "audio.h"

#define MIC_CHAR_COUNT 2056

typedef struct Context {
    const int32_t windowWidth;
    const int32_t windowHeight;
    const int32_t gameWidth;
    const int32_t guiWidth;
    const int32_t guiOffset;
    AudioData data;
    char micList[MIC_CHAR_COUNT];
    int32_t micListUsed;
    int32_t device;
} Context;

#endif // CONTEXT_H
