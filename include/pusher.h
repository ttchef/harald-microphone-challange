
#ifndef PUSHER_H
#define PUSHER_H

#include <stdio.h>
#include <raylib.h>

typedef struct Pusher {
    Vector2 pos;
    Vector2 hitbox;
    float coneAngle;
    float offsetAngle;
    float distance;
    Vector2 pushVel;
    Texture2D tex;
} Pusher;

struct Context;
void addPusher(struct Context* context, Vector2 pos, Vector2 hitbox,
               float coneAngle, float offsetAngle, float distance);
void debugRenderPushersHitbox(struct Context* context);
void renderPushers(struct Context* context);

#endif // PUSHER_H
