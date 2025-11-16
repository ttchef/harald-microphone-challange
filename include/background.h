
#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <raylib.h>

typedef struct Background {
    Texture2D colorLayer; // layer 1
    Texture2D mountainLevelOne; // layer 2 
    Texture2D mountainLevelTwo;

} Background;

struct Context;
void initBackground(Background* bg);
void renderBackground(Background* bg, struct Context* context);
void deinitBackground(Background* bg);

#endif // BACKGROUND_H
