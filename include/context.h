
#ifndef CONTEXT_H
#define CONTEXT_H

#include "audio.h"
#include "game.h"

#define MIC_CHAR_COUNT 2056

typedef struct Context {
    // Window
    const int32_t windowWidth;
    const int32_t windowHeight;
    const int32_t gameWidth;
    const int32_t guiWidth;
    const int32_t guiOffset;

    // Audio
    AudioData data;
    AudioData dataP2;
    char micList[MIC_CHAR_COUNT];
    int32_t deviceCount;
    AudioDevice* devices;
    int32_t activeDevice;
    int32_t activeDeviceP2;
    float volumeMul;
    float volumeMulP2;

    // Game
    GameData gameData;
    bool isMultiplayer;
    bool firstMultiplayerAdd;
    Camera2D camera;
    bool debugMode;
} Context;

#endif // CONTEXT_H
