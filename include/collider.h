
#ifndef COLLIDER_H
#define COLLIDER_H

#include <raylib.h>

typedef enum ColliderType {
    COLLIDER_TYPE_RECTANGLE,
    COLLIDER_TYPE_ONE_WAY_PLATFORM,
    COLLIDER_TYPE_CONE,
} ColliderType;

typedef struct Collider {
    ColliderType type;
    Vector2 pos;
    Vector2 dim;
} Collider;

typedef struct ColliderRect {
    Vector2 pos;
    Vector2 dim;
} ColliderRect;

typedef struct ColliderOneWayPlatform {
    Vector2 pos;
    Vector2 dim;
} ColliderOneWayPlatform;

typedef struct ColliderCone {
    Vector2 pos;
    float coneAngle;
    float offsetAngle;
    float distance;
} ColliderCone;

struct Player;
bool checkAABBPlayer(Collider* collider, struct Player* player);

bool checkAABB(ColliderRect* a, ColliderRect* b);
bool checkOneWayPlatform(ColliderRect* a, ColliderOneWayPlatform platform);

#endif // COLLIDER_H
