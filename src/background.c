
#include "background.h"
#include "context.h"
#include <raylib.h>

void initBackground(Background *bg) {
    bg->colorLayer = LoadTexture("res/BG_DesertMountains/background1.png");  
    bg->mountainLevelOne = LoadTexture("res/BG_DesertMountains/background2.png");
    bg->mountainLevelTwo = LoadTexture("res/BG_DesertMountains/background2.png");
}

static void DrawParallax(Texture2D tex, float camX, float factor, float offsetY) {
    float scroll = fmodf(-camX * factor, tex.width);
    if (scroll > 0) scroll -= tex.width;
    DrawTextureEx(tex, (Vector2){scroll, offsetY}, 0, 2, WHITE);
    DrawTextureEx(tex, (Vector2){scroll + tex.width, offsetY}, 0, 2, WHITE);
}

void renderBackground(Background *bg, Context* context) {
    Vector2 cam = context->camera.target;
    DrawParallax(bg->colorLayer, cam.x, 0.05f, 0);
    DrawParallax(bg->mountainLevelOne, cam.x, 0.1f, -150);
    DrawParallax(bg->mountainLevelTwo, cam.x, 0.2f, -100);
}

void deinitBackground(Background* bg) {
    UnloadTexture(bg->colorLayer);
    UnloadTexture(bg->mountainLevelOne);
    UnloadTexture(bg->mountainLevelTwo);
}

