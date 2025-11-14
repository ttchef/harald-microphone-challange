
#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdatomic.h>

#include <portaudio.h>

#define FRAMES_PER_BUFFER 512

typedef struct AudioDevice {
    int32_t paIndex;
    int32_t micListIndex; 
} AudioDevice;

typedef struct AudioData {
    atomic_int volL;
    atomic_int volR;
    atomic_int requestedChannels;
} AudioData;

struct Context;

// Returns device id
void initAudio(struct Context* context);
void switchDevice(struct Context* context);
void deinitAudio();

static inline void checkErr(PaError err) {
    if (err != paNoError) {
        fprintf(stderr, "PortAdio error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    };
}

#endif // AUDIO_H
