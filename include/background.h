
#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <raylib.h>

#define BACKGROUND_CLOUD_COUNT 8

typedef struct Cloud {
    Texture2D tex;
    Vector2 pos;
    float scale;
} Cloud;

typedef struct Background {
    Texture2D colorLayer; // layer 1
    Texture2D mountainLevelOne; // layer 2 
    Texture2D mountainLevelTwo; // layer 3
    Cloud clouds[BACKGROUND_CLOUD_COUNT];
} Background;

struct Context;
void initBackground(struct Context* context, Background* bg);
void renderBackground(struct Context* context, Background* bg);
void deinitBackground(Background* bg);

#endif // BACKGROUND_H
