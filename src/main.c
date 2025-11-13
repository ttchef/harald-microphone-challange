
#include "audio.h"
#include <raylib.h>


int main() {
    AudioData data = {
        .volL = 0,
        .volR = 0,
    };
    
    int32_t device = initAudio(&data);
    
    const int32_t windowWidth = 1200;
    const int32_t windowHeight = 800;
    const int32_t gameWidth = 900;
    const int32_t guiWidth = 300;
    const int32_t guiOffset = windowWidth - guiWidth;
    InitWindow(windowWidth, windowHeight, "Audio");

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawRectangle(30, 300, atomic_load(&data.volL), 30, RED);
            DrawRectangle(30, 330, atomic_load(&data.volR), 30, RED);
        EndDrawing();
    }

    CloseWindow();
    deinitAudio();

    return 0;
}

