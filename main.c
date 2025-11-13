
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdatomic.h>

#include <raylib.h>
#include <portaudio.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512

typedef struct AudioData {
    atomic_int volL;
    atomic_int volR;
} AudioData;

static inline void checkErr(PaError err) {
    if (err != paNoError) {
        fprintf(stderr, "PortAdio error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    };
}

static int32_t testCallback(const void* inputBuffer, void* outputBuffer, uint64_t framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    float* in = (float*)inputBuffer;

    // Unused Parameters
    (void)outputBuffer;
    (void)timeInfo;
    (void)statusFlags;

    AudioData* data = (AudioData*)userData;

    int32_t dispSize = 100;
    printf("\r");

    float volLeft = 0.0f;
    float volRight = 0.0f;

    for (uint64_t i = 0; i < framesPerBuffer * 2; i += 2) {
        volLeft = fmaxf(volLeft, fabs(in[i]));
        volRight = fmaxf(volRight, fabs(in[i + 1]));
    }

    atomic_store(&data->volL, (int32_t)(volLeft * 400));
    atomic_store(&data->volR, (int32_t)(volRight * 400));

    for (int32_t i = 0; i < dispSize; i++) {
        float barPorportion = i / (float)dispSize;
        if (barPorportion <= volLeft && barPorportion <= volRight) {
            
            printf("█");
        }
        else if (barPorportion <= volLeft) {
            printf("▀");
        }
        else if (barPorportion <= volRight) {
            printf("▄");
        }
        else {
            printf(" ");
        }
    }

    fflush(stdout);
    return 0;
}

int main() {
    PaError err = Pa_Initialize();
    checkErr(err);

    int32_t numDevices = Pa_GetDeviceCount();
    printf("Number of devices: %d\n", numDevices);
    if (numDevices < 0) {
        printf("Error getting device count\n");
        exit(EXIT_FAILURE);
    } 
    else if (numDevices == 0) {
        printf("There are no available audio devices on this machine\n");
        exit(EXIT_SUCCESS);
    }

    PaDeviceInfo* deviceInfo;
    for (int32_t i = 0; i < numDevices; i++) {
        deviceInfo = Pa_GetDeviceInfo(i);
        printf("Found Device: %s\n", deviceInfo->name);
        printf("\tIndex: %d\n", i);
        printf("\tmaxInputChannels: %d\n", deviceInfo->maxInputChannels);
        printf("\tmaxOutputChannels: %d\n", deviceInfo->maxOutputChannels);
        printf("\tdefaultSampleRate: %f\n", deviceInfo->defaultSampleRate);
    }

    int32_t device = 16;
    PaStreamParameters inputParameters = {
        .channelCount = 1,
        .device = device,
        .hostApiSpecificStreamInfo = NULL,
        .sampleFormat = paFloat32,
        inputParameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency,
    };

    PaStreamParameters outputParameters = {
        .channelCount = 1,
        .device = device,
        .hostApiSpecificStreamInfo = NULL,
        .sampleFormat = paFloat32,
        inputParameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency,
    };

    AudioData data = {
        .volL = 0,
        .volR = 0,
    };

    PaStream* stream;
    err = Pa_OpenStream(&stream, &inputParameters, &outputParameters, SAMPLE_RATE, FRAMES_PER_BUFFER, paNoFlag, testCallback, &data);
    checkErr(err);

    err = Pa_StartStream(stream);
    checkErr(err);

    InitWindow(800, 600, "Audio");

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawRectangle(30, 300, atomic_load(&data.volL), 30, RED);
            DrawRectangle(30, 330, atomic_load(&data.volR), 30, RED);
        EndDrawing();
    }

    CloseWindow();

    err = Pa_StopStream(stream);
    checkErr(err);

    err = Pa_CloseStream(stream);
    checkErr(err);

    err = Pa_Terminate();
    checkErr(err);

    return 0;
}

