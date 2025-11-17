
#ifndef COLLIDER_H
#define COLLIDER_H

#include <raylib.h>

typedef enum ColliderType {
    COLLIDER_TYPE_RECTANGLE,
    COLLIDER_TYPE_ONE_WAY_PLATFORM,
} ColliderType;

typedef struct Collider {
    bool isActive;
    ColliderType type;
    Vector2 pos;
    Vector2 dim;
} Collider;

struct Player;
bool checkAABBPlayer(Collider* collider, struct Player* player);

#endif // COLLIDER_H
