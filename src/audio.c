
#include "audio.h"
#include <portaudio.h>
#include <stdlib.h>

PaError err = {0};
PaStream* stream;

static int32_t testCallback(const void* inputBuffer, void* outputBuffer, uint64_t framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    float* in = (float*)inputBuffer;

    // Unused Parameters
    (void)outputBuffer;
    (void)timeInfo;
    (void)statusFlags;

    AudioData* data = (AudioData*)userData;

    float volLeft = 0.0f;
    float volRight = 0.0f;

    for (uint64_t i = 0; i < framesPerBuffer * 2; i += 2) {
        volLeft = fmaxf(volLeft, fabs(in[i]));
        volRight = fmaxf(volRight, fabs(in[i + 1]));
    }

    atomic_store(&data->volL, (int32_t)(volLeft * 400));
    atomic_store(&data->volR, (int32_t)(volRight * 400));

    return 0;
}

int32_t initAudio(AudioData* data) {
    err = Pa_Initialize();
    checkErr(err);

    int32_t device = Pa_GetDefaultInputDevice();
    if (device == paNoDevice) {
        fprintf(stderr, "Failed to find default device!\n");
        exit(EXIT_FAILURE);
    }

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

    err = Pa_OpenStream(&stream, &inputParameters, &outputParameters, SAMPLE_RATE, FRAMES_PER_BUFFER, paNoFlag, testCallback, data);
    checkErr(err);

    err = Pa_StartStream(stream);
    checkErr(err);
    return device;
}

void deinitAudio() {
    err = Pa_StopStream(stream);
    checkErr(err);

    err = Pa_CloseStream(stream);
    checkErr(err);

    err = Pa_Terminate();
    checkErr(err);
}

