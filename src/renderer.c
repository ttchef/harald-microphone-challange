
#include "renderer.h"
#include "context.h"

#include <math.h>

#include <raylib.h>
#include <stdatomic.h>

void renderGui(Context* context) {
    DrawRectangle(context->guiOffset, 0, context->guiWidth, context->windowHeight, DARKGRAY);
    int32_t paddingX = 15;
    int32_t paddingY = 15;
    int32_t currentY = paddingY;
    int32_t startX = context->guiOffset + paddingX;
    int32_t elementWidth = context->guiWidth - 2 * paddingX;

    // Draw Audio Background
    DrawRectangle(startX, currentY, elementWidth, 500, BLACK);
    int32_t volume = fmax(atomic_load(&context->data.volL), atomic_load(&context->data.volR));
    DrawRectangle(startX + paddingX, currentY + paddingY, elementWidth / 2 - paddingX, volume, RED);
}          

