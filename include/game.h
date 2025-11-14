
#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <raylib.h>
#include <raymath.h>

typedef struct Player {
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
    Vector2 dim;
} Player;

typedef struct GameData {
    int32_t groundY;
    Player player;
} GameData;

struct Context;

void initGame(struct Context* context);
void updateGame(struct Context* context, float dt);

#endif // GAME_H
