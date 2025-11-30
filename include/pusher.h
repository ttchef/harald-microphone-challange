
#ifndef PUSHER_H
#define PUSHER_H

#include <stdio.h>
#include <raylib.h>

typedef struct Pusher {
    Vector2 pos;
    Vector2 hitbox;
    float angle;
    float distance;
    float radius;
    Texture2D tex;
} Pusher;

struct Context;
void addPusher(struct Context* context, Vector2 pos, Vector2 hitbox);
void renderPusher(struct Context* context, Pusher* pusher);

#endif // PUSHER_H
