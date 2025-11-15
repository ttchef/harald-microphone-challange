
#include "audio.h"
#include "context.h"

#include <portaudio.h>
#include <stdatomic.h>
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
        "sysdefault", "pipewire", "pulse", "discord",
        "Chromium"
    };

    for (int32_t i = 0; i < ARRAY_COUNT(blacklist); i++) {
        if (strstr(info->name, blacklist[i])) return false;
    }
    return true;
}

static void generateMicList(Context* context, int32_t defaultDevice) {
    // Get Device List
    int32_t deviceCount = Pa_GetDeviceCount();
    if (deviceCount <= 0) return;

    bool first = true;
    int32_t used = 0;
    context->deviceCount = 0;
    context->devices = NULL;
    for (int32_t i = 0; i < deviceCount; i++) {
        const PaDeviceInfo* info = Pa_GetDeviceInfo(i);
        if (!info) continue;   
        if (!isUsefullMic(info)) continue;

        const char* name = info->name;
        size_t nameLen = strlen(name);
        if (used + nameLen >= MIC_CHAR_COUNT) break;

        if (!first) strcat(context->micList, ";");
        strcat(context->micList, info->name);
        used += nameLen;

        // Add to device array
        context->deviceCount++;
        context->devices = realloc(context->devices, sizeof(AudioDevice) * context->deviceCount);
        if (!context->devices) {
            fprintf(stderr, "Failed to allocate devices!\n");
            exit(EXIT_FAILURE);
        }
        context->devices[context->deviceCount - 1].paIndex = i;
        context->devices[context->deviceCount - 1].micListIndex = context->deviceCount - 1;
        
        first = false;
    }

    // Check what is default in micList
    bool found = false;
    for (int32_t i = 0; i < context->deviceCount; i++) {
        if (context->devices[i].paIndex == defaultDevice) {
            context->activeDevice = i;
            found = true;
            break;
        }
    }
    if (!found) {
        fprintf(stderr, "Didnt recognize default device\n");
        exit(EXIT_FAILURE);
    }
}

static int32_t audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    float* in = (float*)inputBuffer;

    // Unused Parameters
    (void)outputBuffer;
    (void)timeInfo;
    (void)statusFlags;

    AudioData* data = (AudioData*)userData;

    float volLeft = 0.0f;
    float volRight = 0.0f;

    if (data->requestedChannels == 1) {
        for (uint64_t i = 0; i < framesPerBuffer; i++) {
            float volume = fabsf(in[i]);
            if (volume > volLeft) volLeft = volume;
         }
    }
    else {
        for (uint64_t i = 0; i < framesPerBuffer * 2; i += 2) {
            volLeft = fmaxf(volLeft, fabs(in[i]));
            volRight = fmaxf(volRight, fabs(in[i + 1]));
        }
    }

    atomic_store(&data->volL, (int32_t)(volLeft * 400));
    atomic_store(&data->volR, (int32_t)(volRight * 400));

    for (uint32_t i = 0; i < framesPerBuffer; i++) {
        float sample = (data->requestedChannels == 1)
                            ? in[i]
                            : 0.5f * (in[2*i] + in[2*i+1]); // stereo → mono
        data->lastSamples[data->sampleWriteIndex] = sample;
        data->sampleWriteIndex = (data->sampleWriteIndex + 1) & 4095;
    }

    int32_t bestLag = 0;
    float bestCorr = 0.0f;

    int32_t sampleRate = atomic_load(&data->sampleRate);
    int32_t minLag = sampleRate / 450;
    int32_t maxLag = sampleRate / 70;

    for (int32_t lag = minLag; lag < maxLag; lag++) {
        float corr = 0.0f;
        for (int32_t i = 0; i < 2048; i++) {
            int32_t idx1 = (data->sampleWriteIndex - 1 - i) & 4095;
            int32_t idx2 = (data->sampleWriteIndex - 1 - i - lag) & 4095;
            corr += data->lastSamples[idx1] * data->lastSamples[idx2];
        }
        if (corr > bestCorr) {
            bestCorr = corr;
            bestLag = lag;
        }
    }

    if (bestLag > 0) {
        float pitchHz = 44100.0f / (float)bestLag;
        uint32_t bits;
        memcpy(&bits, &pitchHz, sizeof(bits));
        atomic_store(&data->pitchBits, bits);

    } else {
        uint32_t bits = 0;
        atomic_store(&data->pitchBits, bits);
    }

    return 0;
}

static void createStream(Context* context, int32_t device) {
    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(device);
    if (!deviceInfo) {
        fprintf(stderr, "Invalid device index: %d\n", device);
        exit(EXIT_FAILURE);
    }

    context->data.requestedChannels = (deviceInfo->maxInputChannels >= 2) ? 2 : 1;
    context->data.sampleRate = deviceInfo->defaultSampleRate;

    PaStreamParameters inputParameters = {
        .channelCount = context->data.requestedChannels,
        .device = device,
        .hostApiSpecificStreamInfo = NULL,
        .sampleFormat = paFloat32,
        inputParameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency,
    };

    err = Pa_OpenStream(&stream, &inputParameters, NULL, deviceInfo->defaultSampleRate, FRAMES_PER_BUFFER, paNoFlag, audioCallback, &context->data);
    checkErr(err);

    err = Pa_StartStream(stream);
    checkErr(err);
}

void initAudio(Context* context) {
    err = Pa_Initialize();
    checkErr(err);

    int32_t device = Pa_GetDefaultInputDevice();
    if (device == paNoDevice) {
        fprintf(stderr, "Failed to find default device!\n");
        exit(EXIT_FAILURE);
    }
    
    generateMicList(context, device);
    createStream(context, device);
}

void switchDevice(Context *context) {
    err = Pa_StopStream(stream);
    checkErr(err);

    err = Pa_CloseStream(stream);
    checkErr(err);

    createStream(context, context->devices[context->activeDevice].paIndex);
}

void deinitAudio() {
    err = Pa_StopStream(stream);
    checkErr(err);

    err = Pa_CloseStream(stream);
    checkErr(err);

    err = Pa_Terminate();
    checkErr(err);
}

float getPitch(Context* context) {
    uint32_t bits = atomic_load(&context->data.pitchBits);
    float pitch;
    memcpy(&pitch, &bits, sizeof(pitch));
    return pitch;
}

