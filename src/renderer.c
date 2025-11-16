
#include "renderer.h"
#include "audio.h"
#include "context.h"
#include "game.h"

#include <math.h>
#include <stdatomic.h>

#include <raylib.h>
#include "raygui.h"

void renderGame(Context *context) {
    GameData* game = &context->gameData;
    DrawRectangleGradientV(0, 0, context->gameWidth, context->windowHeight, BLUE, SKYBLUE);
    DrawRectangleGradientV(0, context->gameData.groundY, context->gameWidth, context->windowHeight - context->gameData.groundY, GREEN, DARKGREEN);

    // Obstacles
    for (int32_t i = 0; i < GAME_MAX_OBSTACLES; i++) {
        if (!game->obstacles[i].isActive) continue;
        Obstacle* obs = &game->obstacles[i];
        DrawRectangle(obs->pos.x, obs->pos.y, obs->dim.x, obs->dim.y, BLACK);
    }

    Rectangle player = {
        .x = game->player.pos.x,
        .y = game->player.pos.y,
        .width = game->player.dim.x,
        .height = game->player.dim.y,
    };
    DrawRectangleRounded(player, 0.2f, 10, RAYWHITE);

    if (context->isMultiplayer) {
        player.x = game->player2.pos.x;
        player.y = game->player2.pos.y;
        player.width = game->player2.dim.x;
        player.height = game-> player2.dim.y;
        DrawRectangleRounded(player, 0.2f, 10, PINK);
    }

    drawRope(&game->rope);
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

    // Multiplayer text
    const char* text = TextFormat("Multiplayer: %s\n", context->isMultiplayer ? "on " : "off");
    int32_t fontSize = 20;
    int32_t textWidth = MeasureText(text, fontSize);
    while (textWidth > elementWidth) {
        fontSize--;
        textWidth = MeasureText(text, fontSize);
    }
    int32_t multiplayerWidth = MeasureText("Multiplayer: ", fontSize);
    DrawRectangle(startX + multiplayerWidth, currentY, elementWidth - multiplayerWidth - paddingX / 2, fontSize, context->isMultiplayer ? DARKGREEN : RED);
    DrawText(text, startX, currentY, fontSize, RAYWHITE);
    currentY += fontSize + paddingY;

    // Multiplayer button
    Rectangle bounds2 = { .x = startX, .y = currentY, .width = elementWidth, .height = buttonHeight };
    if (GuiButton(bounds2, "Toggle Multiplayer")) context->isMultiplayer = !context->isMultiplayer;
    currentY += buttonHeight + paddingY;   

    // Draw Audio Background
    int32_t maxHeight = 500;
    DrawRectangle(startX, currentY, elementWidth, maxHeight, BLACK);
    int32_t volume = fmin(fmax(atomic_load(&context->data.volL), atomic_load(&context->data.volR)), maxHeight - paddingY * 2);
    volume = volume <= 10 ? 0 : volume;

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

    Rectangle bounds3 = { .x = startX, .y = currentY, .width = elementWidth, .height = buttonHeight };
    static int32_t active2 = 0;
    static int32_t oldActive2 = 0;
    static bool editMode2 = false;
    currentY += buttonHeight + paddingY;
    active2 = first ? context->activeDevice : active2;

    // Multiplayer text
    const char* text = TextFormat("Multiplayer: %s\n", context->isMultiplayer ? "on " : "off");
    int32_t fontSize = 20;
    int32_t textWidth = MeasureText(text, fontSize);
    while (textWidth > elementWidth) {
        fontSize--;
        textWidth = MeasureText(text, fontSize);
    }
    int32_t multiplayerWidth = MeasureText("Multiplayer: ", fontSize);
    DrawRectangle(startX + multiplayerWidth, currentY, elementWidth - multiplayerWidth - paddingX / 2, fontSize, context->isMultiplayer ? DARKGREEN : RED);
    DrawText(text, startX, currentY, fontSize, RAYWHITE);
    currentY += fontSize + paddingY;

    // Multiplayer button
    Rectangle bounds2 = { .x = startX, .y = currentY, .width = elementWidth, .height = buttonHeight };
    if (GuiButton(bounds2, "Toggle Multiplayer")) context->isMultiplayer = !context->isMultiplayer;
    currentY += buttonHeight + paddingY;   

    // Draw Audio Background
    int32_t maxHeight = 500;
    DrawRectangle(startX, currentY, elementWidth, maxHeight, BLACK);
    int32_t volume = fmin(fmax(atomic_load(&context->data.volL), atomic_load(&context->data.volR)), maxHeight - paddingY * 2);
    volume = volume <= 10 ? 0 : volume;

    int32_t volumeP2 = fmin(fmax(atomic_load(&context->dataP2.volL), atomic_load(&context->dataP2.volR)), maxHeight - paddingY * 2);
    volumeP2 = volumeP2 <= 10 ? 0 : volumeP2;

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

