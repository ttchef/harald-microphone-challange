
#include "audio.h"
#include "game.h"
#include "context.h"
#include "renderer.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <raylib.h>

int main() {
    Context context = {
        .windowWidth = 1200,
        .windowHeight = 800,
        .gameWidth = 1000,
        .guiWidth = 200,
        .guiOffset = 1200 - 200,
        .firstMultiplayerAdd = true,
        .camera = { .zoom = 1.0f},
    };
    
    InitWindow(context.windowWidth, context.windowHeight, "Audio");
    SetTargetFPS(60);
    initAudio(&context);
    initGame(&context);

    while (!WindowShouldClose()) {
        updateGame(&context, GetFrameTime());
        BeginDrawing();
            ClearBackground(BLACK);

            renderGameNoCamera(&context);
            BeginMode2D(context.camera);
                renderGame(&context);
            EndMode2D();

            renderGui(&context);
        EndDrawing();
    }

    CloseWindow();
    deinitGame(&context);
    deinitAudio();

    return 0;
}


