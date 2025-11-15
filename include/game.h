
#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <raylib.h>
#include <raymath.h>

#define PLAYER_STRENGTH_COOLDOWN (0.12f)
#define PLAYER_VOLUME_VELO_FACTOR (1.3)
#define PLAYER_JUMP_THRESHOLD (180)
#define GAME_GRAVITY (800.0f)
#define GROUND_FRICTION (5.0f)
#define GAME_MAX_OBSTACLES 20

typedef struct Player {
    bool onGround;
    bool isJumping;
    float volPeak;
    float strengthCooldown;
    float pitchFiltered;
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
    Vector2 dim;
} Player;

typedef struct Obstacle {
    bool isActive;
    Vector2 pos;
    Vector2 dim;
} Obstacle;

typedef struct GameData {
    int32_t groundY;
    Player player;
    Obstacle obstacles[GAME_MAX_OBSTACLES];
} GameData;

struct Context;

void initGame(struct Context* context);
void updateGame(struct Context* context, float dt);

#endif // GAME_H
