
#include "game.h"
#include "context.h"
#include <raymath.h>

void initGame(Context *context) {
    GameData* data = &context->gameData;
    data->groundY = context->windowHeight * 0.9f;

    // Player
    data->player.pos = (Vector2){context->windowWidth * 0.2f, context->windowHeight * 0.5f};
    data->player.dim = (Vector2){40, 40};
}

static void updatePlayer(GameData* gameData, float dt) {
    Player* p = &gameData->player;
    
    p->acc.y = 0.1f;
    p->vel = Vector2Add(p->vel, p->acc);
    p->pos = Vector2Add(p->pos, p->vel);

    if (p->pos.y + p->dim.y > gameData->groundY && p->vel.y > 0) {
        p->vel.y *= -1;
        p->pos.y = gameData->groundY - p->dim.y;
    }
}

void updateGame(Context *context, float dt) {
    updatePlayer(&context->gameData, dt);
}


