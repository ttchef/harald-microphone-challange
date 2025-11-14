
#include "audio.h"
#include "context.h"
#include "renderer.h"
#include <raylib.h>

int main() {
    Context context = {
        .windowWidth = 1200,
        .windowHeight = 800,
        .gameWidth = 1000,
        .guiWidth = 200,
        .guiOffset = 1200 - 200,
    };
    
    InitWindow(context.windowWidth, context.windowHeight, "Audio");

    int32_t device = initAudio(&context.data);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            renderGui(&context);
        EndDrawing();
    }

    CloseWindow();
    deinitAudio();

    return 0;
}


