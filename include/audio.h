
#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdatomic.h>

#include <portaudio.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512

typedef struct AudioData {
    atomic_int volL;
    atomic_int volR;
} AudioData;

struct Context;
// Returns device id
int32_t initAudio(struct Context* context);
void deinitAudio();

static inline void checkErr(PaError err) {
    if (err != paNoError) {
        fprintf(stderr, "PortAdio error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    };
}

#endif // AUDIO_H
