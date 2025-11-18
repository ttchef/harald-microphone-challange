
#ifndef GAME_H
#define GAME_H

#include <rope.h>
#include <collider.h>
#include <background.h>

#include <stdint.h>
#include <raylib.h>
#include <raymath.h>

#define PLAYER_STRENGTH_COOLDOWN (0.12f)
#define PLAYER_VOLUME_VELO_FACTOR (1.3)
#define PLAYER_JUMP_THRESHOLD (180)
#define PLAYER_SCALE (3)
#define GROUND_SCALE (4)
#define PLATFORM_SCALE (2)
#define GAME_GRAVITY (800.0f)
#define GROUND_FRICTION (15.0f)
#define GAME_MAX_OBSTACLES 20

typedef enum PlayerDirection {
    PLAYER_DIRECTION_RIGHT,
    PLAYER_DIRECTION_LEFT,
} PlayerDirection;

typedef enum PlayerAnimationState {
    PLAYER_ANIM_STATE_IDLE,
    PLAYER_ANIM_STATE_RUN,
    PLAYER_ANIM_STATE_PUNCH,
    PLAYER_ANIM_STATE_JUMP,
    PLAYER_ANIM_STATE_DEATH,
    PLAYER_ANIM_STATE_NUM
} PlayerAnimationState;

typedef struct Player {
    bool onGround;
    bool isJumping;
    float volPeak;
    float strengthCooldown;
    float pitchFiltered;
    Vector2 oldPos;
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
    Vector2 dim;
    Vector2 hitbox;
    Vector2 hitboxOrigin;
    PlayerDirection direction;
    Texture2D texture;
    int32_t framesSpeed;
    int32_t currentFrame;
    Rectangle frameRec;
    int32_t frameCounter;
    Color accentColor;
    int32_t texRow;
    int32_t texColumn;
    PlayerAnimationState animState;
} Player;

typedef struct Platform {
    bool isActive;
    Collider collider;
    Texture2D middle;
    Texture2D left;
    Texture2D right;
} Platform;

typedef struct Obstacle {
    bool isActive;
    Vector2 pos;
    Vector2 dim;
} Obstacle;

typedef struct GameData {
    Background bg;
    int32_t groundY;
    Texture2D groundTex;
    Player player;
    Player player2;
    Obstacle obstacles[GAME_MAX_OBSTACLES];
    Platform platforms[GAME_MAX_OBSTACLES];
    Rope rope;
} GameData;

struct Context;

void initGame(struct Context* context);
void updateGame(struct Context* context, float dt);
void deinitGame(struct Context* context);

#endif // GAME_H
