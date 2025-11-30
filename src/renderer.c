
#include "renderer.h"
#include "audio.h"
#include "background.h"
#include "collider.h"
#include "context.h"
#include "game.h"

#include <math.h>
#include <stdatomic.h>

#include <raylib.h>
#include "particle.h"
#include "raygui.h"

static void debugRenderPlayerHitbox(Context* context, Player* p) {
    GameData* game = &context->gameData;
    DrawRectangleLines(p->pos.x + p->hitboxOrigin.x, p->pos.y + p->hitboxOrigin.y, p->hitbox.x, p->hitbox.y, GREEN);
}

static void debugRenderPlatformHitbox(Context* context) {
    GameData* game = &context->gameData;
    for (int32_t i = 0; i < GAME_MAX_OBSTACLES; i++) {
        if (!game->platforms[i].isActive) continue;
        Platform* p = &game->platforms[i];
        Collider* c = &p->collider;
        DrawRectangleLines(c->pos.x, c->pos.y, c->dim.x, c->dim.y, GREEN);
    }
}

static void renderGround(Context* context) {
    GameData* game = &context->gameData;

    int32_t drawCount = (context->gameWidth * 4) / game->groundTex.width * GROUND_SCALE;
    DrawRectangle(-context->gameWidth * 2, game->groundY, context->gameWidth * 5, context->windowHeight * 0.4f, (Color){33, 25, 25, 255}); // Color of last pixel from ground tex
    for (int32_t i = -context->gameWidth * 2; i < context->gameWidth * 3; i += game->groundTex.width * GROUND_SCALE) {
        DrawTextureEx(game->groundTex, (Vector2){i, game->groundY - game->groundTex.height * GROUND_SCALE + 17 * GROUND_SCALE}, 0, GROUND_SCALE, WHITE);
    }
}

static void renderPlatforms(Context* context) {
    GameData* game = &context->gameData;
    for (int32_t i = 0; i < GAME_MAX_OBSTACLES; i++) {
        if (!game->platforms[i].isActive) continue;
        Platform* p = &game->platforms[i];
        Collider* c = &p->collider;
     
        DrawTextureEx(p->middle, (Vector2){c->pos.x + p->left.width * PLATFORM_SCALE, c->pos.y}, 0, PLATFORM_SCALE, WHITE);
        DrawTextureEx(p->left, (Vector2){c->pos.x, c->pos.y}, 0, PLATFORM_SCALE, WHITE);
        DrawTextureEx(p->right, (Vector2){c->pos.x - 2 + (p->left.width + p->middle.width) * PLATFORM_SCALE, c->pos.y}, 0, PLATFORM_SCALE, WHITE);
    }
}

static void renderPlayer(Player* p) {
    Vector2 origin = {0};

    Rectangle srcRec = p->frameRec;

    Rectangle destRec = {
        .x = p->pos.x,
        .y = p->pos.y,
        .width = p->frameRec.width * PLAYER_SCALE,
        .height = p->frameRec.height * PLAYER_SCALE,
    };

    if (p->direction == PLAYER_DIRECTION_LEFT) {
        srcRec.width = -srcRec.width;
        destRec.x -= 10;
    }

    DrawTexturePro(p->texture, srcRec, destRec, origin, 0.0f, WHITE);
}

void renderGame(Context *context) {
    GameData* game = &context->gameData;
    renderGround(context);
    drawParticleSystem(context);

    // Obstacles
    for (int32_t i = 0; i < GAME_MAX_OBSTACLES; i++) {
        if (!game->obstacles[i].isActive) continue;
        Obstacle* obs = &game->obstacles[i];
        DrawRectangle(obs->pos.x, obs->pos.y, obs->dim.x, obs->dim.y, BLACK);
    }

    // Colliders
    renderPlatforms(context);

    // Pushers
    for (int32_t i = 0; i < darrayLength(game->pushers); i++) {
        renderPusher(context, &game->pushers[i]);
    }

    renderPlayer(&game->player);
    if (context->isMultiplayer) {
        renderPlayer(&game->player2);
        drawRope(&game->rope);
    }

    if (context->debugMode) {
        debugRenderPlayerHitbox(context, &game->player);
        debugRenderPlatformHitbox(context);
    }     
    if (context->debugMode && context->isMultiplayer)  debugRenderPlayerHitbox(context, &game->player2);
}

void renderGameNoCamera(Context *context) {
    GameData* game = &context->gameData;
    renderBackground(context, &game->bg);
    //DrawRectangleGradientV(0, 0, context->gameWidth, context->windowHeight, BLUE, SKYBLUE);
    //DrawTexture(LoadTexture("res/PirateBomb/Sprites/7-Objects/1-BOMB/2-Bomb On/1.png"), 300, 300, WHITE);
}

void renderGuiSinglePlayer(Context* context) {
    DrawRectangle(context->guiOffset, 0, context->guiWidth, context->windowHeight, DARKGRAY);
    int32_t paddingX = 15;
    int32_t paddingY = 15;
    int32_t currentY = paddingY;
    int32_t startX = context->guiOffset + paddingX;
    int32_t elementWidth = context->guiWidth - 2 * paddingX;

    // Select mic dropdown menu
    int32_t buttonHeight = 30;
    Rectangle bounds = { .x = startX, .y = currentY, .width = elementWidth, .height = buttonHeight };
    static bool first = true;
    static int32_t active = 0;
    static int32_t oldActive = 0;
    static bool editMode = false;
    currentY += buttonHeight + paddingY;
    active = first ? context->activeDevice : active;

    Rectangle sliderBounds = bounds;
    sliderBounds.y = currentY;
    GuiSlider(sliderBounds, "", "", &context->volumeMul, 1, 2);
    currentY += buttonHeight + paddingY;

    // Multiplayer text
    const char* text = "Multiplayer";
    int32_t fontSize = 20;
    int32_t textWidth = MeasureText(text, fontSize);
    while (textWidth > elementWidth) {
        fontSize--;
        textWidth = MeasureText(text, fontSize);
    }
    int32_t multiplayerWidth = MeasureText("Multiplayer: ", fontSize);
    Rectangle toggleSliderBounds = { .x = startX, currentY, elementWidth - multiplayerWidth, fontSize};
    GuiCheckBox(toggleSliderBounds, "   Multiplayer", &context->isMultiplayer);
    //DrawText(text, startX, currentY, fontSize, RAYWHITE);
    currentY += fontSize + paddingY;

    // Multiplayer button
    Rectangle bounds2 = { .x = startX, .y = currentY, .width = elementWidth, .height = buttonHeight };
    if (GuiButton(bounds2, "Toggle Multiplayer")) context->isMultiplayer = !context->isMultiplayer;
    currentY += buttonHeight + paddingY;   

    // Draw Audio Background
    int32_t maxHeight = 500;
    DrawRectangle(startX, currentY, elementWidth, maxHeight, BLACK);
    int32_t volume = fmin(fmax(atomic_load(&context->data.volL), atomic_load(&context->data.volR)), maxHeight - paddingY * 2) * context->volumeMul;
    volume = volume <= 10 ? 0 : volume;
    if (volume > 400) volume = 400;

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

    currentY += maxHeight + paddingY;
    
    // Render drop down menu on top
    if (GuiDropdownBox(bounds, context->micList, &active, editMode)) editMode = !editMode;
    if (active != oldActive) {
        context->activeDevice = active;
        switchDevice(context, PLAYER_1);
        oldActive = active;
    }
    first = false;


    // Overlap with game
    startX = context->guiOffset - paddingX - elementWidth;
    currentY = paddingY;
    DrawText(TextFormat("Highscore: %d", context->gameData.maxHeight), startX, currentY, 20, RAYWHITE);
    currentY += paddingY + 20;
    DrawText(TextFormat("Height: %d", context->gameData.height), startX, currentY, 20, RAYWHITE);
    currentY += paddingY + 20;
}          

void renderGuiMultiplayer(Context* context) {
    DrawRectangle(context->guiOffset, 0, context->guiWidth, context->windowHeight, DARKGRAY);
    int32_t paddingX = 15;
    int32_t paddingY = 15;
    int32_t currentY = paddingY;
    int32_t startX = context->guiOffset + paddingX;
    int32_t elementWidth = context->guiWidth - 2 * paddingX;

    // Select mic dropdown menu
    int32_t buttonHeight = 30;
    Rectangle bounds = { .x = startX, .y = currentY, .width = elementWidth, .height = buttonHeight };
    static bool first = true;
    static int32_t active = 0;
    static int32_t oldActive = 0;
    static bool editMode = false;
    currentY += buttonHeight + paddingY;
    active = first ? context->activeDevice : active;

    Rectangle sliderBounds = bounds;
    sliderBounds.y = currentY;
    GuiSlider(sliderBounds, "", "", &context->volumeMul, 1, 5);
    currentY += buttonHeight + paddingY;

    Rectangle bounds3 = { .x = startX, .y = currentY, .width = elementWidth, .height = buttonHeight };
    static int32_t active2 = 0;
    static int32_t oldActive2 = 0;
    static bool editMode2 = false;
    currentY += buttonHeight + paddingY;
    active2 = first ? context->activeDevice : active2;

    sliderBounds = bounds;
    sliderBounds.y = currentY;
    GuiSlider(sliderBounds, "", "", &context->volumeMulP2, 1, 5);
    currentY += buttonHeight + paddingY;

    // Multiplayer text
    const char* text = "Multiplayer";
    int32_t fontSize = 20;
    int32_t textWidth = MeasureText(text, fontSize);
    while (textWidth > elementWidth) {
        fontSize--;
        textWidth = MeasureText(text, fontSize);
    }
    int32_t multiplayerWidth = MeasureText("Multiplayer: ", fontSize);
    Rectangle toggleSliderBounds = { .x = startX, currentY, elementWidth - multiplayerWidth, fontSize};
    GuiCheckBox(toggleSliderBounds, "   Multiplayer", &context->isMultiplayer);
    //DrawText(text, startX, currentY, fontSize, RAYWHITE);
    currentY += fontSize + paddingY;   

    // Draw Audio Background
    int32_t maxHeight = 500;
    DrawRectangle(startX, currentY, elementWidth, maxHeight, BLACK);
    int32_t volume = fmin(fmax(atomic_load(&context->data.volL), atomic_load(&context->data.volR)), maxHeight - paddingY * 2) * context->volumeMul;
    volume = volume <= 10 ? 0 : volume;
    if (volume > 400) volume = 400;

    int32_t volumeP2 = fmin(fmax(atomic_load(&context->dataP2.volL), atomic_load(&context->dataP2.volR)), maxHeight - paddingY * 2) * context->volumeMulP2;
    volumeP2 = volumeP2 <= 10 ? 0 : volumeP2;
    if (volumeP2 > 400) volumeP2 = 400;

    int32_t barX = startX + elementWidth / 8;
    int32_t barW = elementWidth / 4 + paddingX;
    int32_t barYbottom = currentY + maxHeight - paddingY;

    // Player 1
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

    // Player 2
    for (int32_t i = 0; i < volumeP2; i++) {
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
        DrawRectangle(barX + barW + paddingX, barYbottom - i, barW, 1, color);
    }

    currentY += maxHeight + paddingY;
    
    // Render drop down menu on top
    if (GuiDropdownBox(bounds3, context->micList, &active2, editMode2)) editMode2 = !editMode2;
    if (active2 != oldActive2) {
        context->activeDeviceP2 = active2;
        switchDevice(context, PLAYER_2);
        if (context->firstMultiplayerAdd) context->firstMultiplayerAdd = false;
        oldActive2 = active2;
    }

    if (GuiDropdownBox(bounds, context->micList, &active, editMode)) editMode = !editMode;
    if (active != oldActive) {
        context->activeDevice = active;
        switchDevice(context, PLAYER_1);
        oldActive = active;
    }
    first = false;
}          

void renderGui(Context* context) {
    if (context->isMultiplayer) renderGuiMultiplayer(context);
    else renderGuiSinglePlayer(context);
}

