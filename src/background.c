
#include "background.h"
#include "context.h"
#include <string.h>
#include <raylib.h>

void initBackground(Context* context, Background* bg) {
    bg->colorLayer = LoadTexture("res/BG_DesertMountains/background1.png");  
    bg->mountainLevelOne = LoadTexture("res/BG_DesertMountains/background2.png");
    bg->mountainLevelTwo = LoadTexture("res/BG_DesertMountains/background2.png");

    char path[256];
    for (int32_t i = 0; i < BACKGROUND_CLOUD_COUNT; i++) {
        snprintf(path, sizeof(path), "res/BG_DesertMountains/cloud%d.png", i);
        bg->clouds[i].tex = LoadTexture(path);

        int32_t randX = GetRandomValue(0, context->gameWidth - bg->clouds[i].tex.width);
        int32_t randY = GetRandomValue(0, context->windowHeight - bg->clouds[i].tex.height);
        float scale = GetRandomValue(20, 100) / 33.0f;
        bg->clouds[i].scale = scale;
        bg->clouds[i].pos = (Vector2){randX, randY};
    }
}

static void DrawParallax(Texture2D tex, float camX, float factor, float offsetY) {
    float scroll = fmodf(-camX * factor, tex.width);
    if (scroll > 0) scroll -= tex.width;
    DrawTextureEx(tex, (Vector2){scroll, offsetY}, 0, 2, WHITE);
    DrawTextureEx(tex, (Vector2){scroll + tex.width, offsetY}, 0, 2, WHITE);
}

void renderBackground(Context* context, Background* bg) {
    Vector2 cam = context->camera.target;
    DrawParallax(bg->colorLayer, cam.x, 0.05f, 0);
    DrawParallax(bg->mountainLevelOne, cam.x, 0.1f, -150);
    DrawParallax(bg->mountainLevelTwo, cam.x, 0.2f, -100);

    for (int32_t i = 0; i < BACKGROUND_CLOUD_COUNT; i++) {
        float factor = 0.2f;
        float offsetX = -cam.x * factor;
        Vector2 pos = (Vector2){bg->clouds[i].pos.x + offsetX, bg->clouds[i].pos.y};
        DrawTextureEx(bg->clouds[i].tex, pos, 0, bg->clouds[i].scale, WHITE);
    }
}

void deinitBackground(Background* bg) {
    UnloadTexture(bg->colorLayer);
    UnloadTexture(bg->mountainLevelOne);
    UnloadTexture(bg->mountainLevelTwo);

    for (int32_t i = 0; i < BACKGROUND_CLOUD_COUNT; i++) {
        UnloadTexture(bg->clouds[i].tex);
    }
}

