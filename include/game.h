
#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdbool.h>

typedef struct GameData {
    int32_t groundY;
} GameData;

struct Context;

void initGame(struct Context* context);

#endif // GAME_H
