#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <SDL3/SDL.h>

int32_t screenWidth = 800;
int32_t screenHeight = 600;

typedef struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Color;

typedef struct Player {
    float x;
    float y;
    float veloX;
    float veloY;
    float accX;
    float accY;
    int32_t width;
    int32_t height;
    Color color;
    bool collided;
    float groundThreshold;
} Player;

void jumpPlayer(Player* player) {
    // Only jump if player is on or very close to the ground
    if (player->y >= player->groundThreshold) {
        player->veloY = -400.0f;  // Immediate upward velocity
    }
}

void updatePlayer(Player* player, int32_t groundY, float dt) {
    player->collided = false;

    const float gravity = 25.0f;
    const float friction = 0.85f;
    player->accY += gravity;

    // Update velocity (pixels/s)
    player->veloX += player->accX * dt;
    player->veloY += player->accY * dt;

    // Apply friction to horizontal velocity
    player->veloX *= friction;

    // Terminal velocity (pixels/s)
    const float terminalVelocity = 500.0f;
    if (player->veloY > terminalVelocity) {
        player->veloY = terminalVelocity;
    }
    if (player->veloY < -terminalVelocity) {
        player->veloY = -terminalVelocity;
    }

    // Update position (pixels)
    player->x += player->veloX * dt;
    player->y += player->veloY * dt;

    // Keep player in bounds
    if (player->x < 0) {
        player->x = 0;
        player->veloX = 0;
    }
    if (player->x + player->width > screenWidth) {
        player->x = screenWidth - player->width;
        player->veloX = 0;
    }

    // Ground collision
    if (player->y + player->height > groundY && player->veloY > 0) {
        player->y = groundY - player->height;
        player->veloY = 0.0f;
        player->accY = 0.0f;
        player->collided = true;
    }
}

void renderPlayer(SDL_Renderer* renderer, Player* player) {
    SDL_SetRenderDrawColor(renderer, player->color.red, player->color.green, player->color.blue, 255);
    SDL_FRect playerSquare = {
        .x = player->x,
        .y = player->y,
        .w = player->width,
        .h = player->height,
    };
    SDL_RenderFillRect(renderer, &playerSquare);
}

void renderGround(SDL_Renderer* renderer, int32_t groundY) {
    SDL_SetRenderDrawColor(renderer, 30, 255, 30, 255);
    SDL_FRect square = {
        .x = 0,
        .y = groundY, 
        .w = screenWidth,
        .h = screenHeight - groundY,
    };
    SDL_RenderFillRect(renderer, &square);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Failed to init SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Audio Recorder", screenWidth, screenHeight, SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "Failed to create SDL window: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        fprintf(stderr, "Failed to create SDL renderer: %s\n", SDL_GetError());
        return -1;
    }

    int32_t deviceCount = 0;
    SDL_AudioDeviceID* devices = SDL_GetAudioRecordingDevices(&deviceCount);
    if (deviceCount < 1) {
        fprintf(stderr, "Failed to get any microphones\n");
        return -1;
    }

    for (int32_t i = 0; i < deviceCount; i++) {
        printf("Audio Device %d: %s\n", i, SDL_GetAudioDeviceName(devices[i]));
    }

    int32_t deviceIndex = 0;
    const char* devName = SDL_GetAudioDeviceName(devices[deviceIndex]);
    printf("Using device: %s\n", devName);

    SDL_AudioSpec desired = {0};
    desired.freq = 44100;
    desired.format = SDL_AUDIO_F32;
    desired.channels = 1;

    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(devices[deviceIndex], &desired);
    if (dev == 0) {
        fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
        return -1;
    }

    printf("Opened audio device successfully (ID: %u)\n", dev);

    SDL_AudioSpec streamSpec = {0};
    streamSpec.freq = 44100;
    streamSpec.format = SDL_AUDIO_F32;
    streamSpec.channels = 1;

    SDL_AudioStream* stream = SDL_CreateAudioStream(&desired, &streamSpec);
    if (!stream) {
        fprintf(stderr, "Failed to create audio stream: %s\n", SDL_GetError());
        SDL_CloseAudioDevice(dev);
        return -1;
    }

    if (!SDL_BindAudioStream(dev, stream)) {
        fprintf(stderr, "Failed to bind audio stream: %s\n", SDL_GetError());
        SDL_DestroyAudioStream(stream);
        SDL_CloseAudioDevice(dev);
        return -1;
    }

    if (!SDL_ResumeAudioDevice(dev)) {
        fprintf(stderr, "Failed to resume audio device: %s\n", SDL_GetError());
        SDL_DestroyAudioStream(stream);
        SDL_CloseAudioDevice(dev);
        return -1;
    }

    printf("Recording started...\n");
    SDL_Delay(1000); 

    int32_t running = 1;
    SDL_Event event;

    int32_t groundY = screenHeight * 0.9;
    
    Player player = {0};
    player.width = 40;
    player.height = 40;
    player.x = screenWidth * 0.1;
    player.y = groundY - player.height;
    player.groundThreshold = groundY - player.height - 5;  // Small buffer for jumping
    player.color = (Color){255, 255, 255};

    uint64_t NOW = SDL_GetPerformanceCounter();
    uint64_t LAST = NOW;
    float deltaTime = 0.0f;
    while (running) {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (float)((NOW - LAST) / (float)SDL_GetPerformanceFrequency());
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }

        const bool* keyboardState = SDL_GetKeyboardState(NULL);
        if (keyboardState[SDL_SCANCODE_E]) {
            jumpPlayer(&player);
        }

        // Horizontal movement
        if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT]) {
            player.accX += -200.0f;  // Move left
        } else if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT]) {
            player.accX += 200.0f;   // Move right
        } else {
            player.accX = 0.0f;    // Stop horizontal acceleration
        }

        int available = SDL_GetAudioStreamAvailable(stream);
        //printf("Available bytes: %d\n", available);

        if (available > 0) {
            float buffer[1024];
            int bytesToRead = available < (int)sizeof(buffer) ? available : (int)sizeof(buffer);
            int bytesRead = SDL_GetAudioStreamData(stream, buffer, bytesToRead);

            if (bytesRead > 0) {
                int sampleCount = bytesRead / sizeof(float);
                float sum = 0.0f;

                for (int i = 0; i < sampleCount; i++) {
                    sum += buffer[i] * buffer[i];
                }

                float rms = sqrtf(sum / sampleCount);
                if (rms > 0.3) jumpPlayer(&player);
                //printf("Volume: %.4f (%d samples)\n", rms, sampleCount);
            }
        }

        updatePlayer(&player, groundY, deltaTime);

        SDL_SetRenderDrawColor(renderer, 30, 30, 60, 255);
        SDL_RenderClear(renderer);
        
        renderGround(renderer, groundY);
        renderPlayer(renderer, &player);

        SDL_RenderPresent(renderer);

        SDL_Delay(16); 
    }

    SDL_DestroyAudioStream(stream);
    SDL_CloseAudioDevice(dev);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

