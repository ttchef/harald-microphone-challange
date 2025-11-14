
#include "game.h"
#include "context.h"
#include <raymath.h>
#include <stdatomic.h>

void initGame(Context *context) {
    GameData* data = &context->gameData;
    data->groundY = context->windowHeight * 0.9f;

    // Player
    data->player.pos = (Vector2){context->windowWidth * 0.2f, context->windowHeight * 0.5f};
    data->player.dim = (Vector2){40, 40};
}

static void updatePlayer(Context *context, float dt) {
    GameData* gameData = &context->gameData;
    Player* p = &gameData->player;
    
    // Over threshold?
    int32_t volume = fmaxf(atomic_load(&context->data.volR), atomic_load(&context->data.volL));
    if (p->onGround && volume > 200) {
        p->vel.y = -600.0f;
    }

    p->acc.y = 500.0f;
    p->vel = Vector2Add(p->vel, Vector2Scale(p->acc, dt));
    p->pos = Vector2Add(p->pos, Vector2Scale(p->vel, dt));

    p->onGround = false;
    if (p->pos.y + p->dim.y > gameData->groundY && p->vel.y > 0) {
        p->pos.y = gameData->groundY - p->dim.y;
        p->onGround = true;
    }
}

void updateGame(Context *context, float dt) {
    updatePlayer(context, dt);
}


