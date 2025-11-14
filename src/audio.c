
#include "audio.h"
#include "context.h"

#include <portaudio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

PaError err = {0};
PaStream* stream;

#define ARRAY_COUNT(arr) ((sizeof((arr)) / sizeof((arr)[0])))

static bool isUsefullMic(const PaDeviceInfo* info) {
    if (info->maxInputChannels <= 0) return false;

    const char* blacklist[] = {
        "speex", "upmix", "vdownmix", "samplerate",
        "speexrate", "lavrate", "webrtc", "hdmi",
        "HDA NVidia", "TU116", "hw:", "Zen", "jack",
        "sysdefault", "pipewire", "pulse"
    };

    for (int32_t i = 0; i < ARRAY_COUNT(blacklist); i++) {
        if (strcasestr(info->name, blacklist[i])) return false;
    }
    return true;
}

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

int32_t initAudio(Context* context) {
    err = Pa_Initialize();
    checkErr(err);

    int32_t device = Pa_GetDefaultInputDevice();
    if (device == paNoDevice) {
        fprintf(stderr, "Failed to find default device!\n");
        exit(EXIT_FAILURE);
    }
    context->device = device;

    // Get Device List
    int32_t deviceCount = Pa_GetDeviceCount();
    if (deviceCount) {
        bool first = true;
        for (int32_t i = 0; i < deviceCount; i++) {
            const PaDeviceInfo* info = Pa_GetDeviceInfo(i);
            if (!info) continue;   
            if (!isUsefullMic(info)) continue;

            const char* name = info->name;
            size_t nameLen = strlen(name);
            if (context->micListUsed + nameLen >= MIC_CHAR_COUNT) break;

            if (!first) strcat(context->micList, ";");
            strcat(context->micList, info->name);
            context->micListUsed += nameLen;
            first = false;
        }
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

    err = Pa_OpenStream(&stream, &inputParameters, &outputParameters, SAMPLE_RATE, FRAMES_PER_BUFFER, paNoFlag, testCallback, &context->data);
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

