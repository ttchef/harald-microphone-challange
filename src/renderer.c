
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
    int32_t maxHeight = 500;
    DrawRectangle(startX, currentY, elementWidth, maxHeight, BLACK);
    int32_t volume = fmin(fmax(atomic_load(&context->data.volL), atomic_load(&context->data.volR)), maxHeight);

    int32_t barX = startX + elementWidth / 4;
    int32_t barW = elementWidth / 2;
    int32_t barYbottom = currentY + maxHeight - paddingY;

    for (int32_t i = 0; i < volume; i++) {
        float t = i / (float)maxHeight;
        
        Color color;
        if (t < 0.5f) {
            float k = t / 0.5f;
            color = (Color){(uint8_t)(k * 255), 255, 0, 255};
        }
        else {
            float k = (t - 0.5f) / 0.5f;
            color = (Color){255, (uint8_t)((1.0f - k) * 255), 0, 255};
        }
        DrawRectangle(barX, barYbottom - i, barW, 1, color);
    }

    //DrawRectangle(startX + elementWidth / 4, currentY + maxHeight - paddingY - volume, elementWidth / 2, volume, RED);
    currentY += maxHeight + paddingY;
}          

