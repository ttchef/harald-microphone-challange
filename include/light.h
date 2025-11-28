
#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>
#include <raylib.h>

typedef struct ShadowGeometry {
    Vector2 vertices[4];
} ShadowGeometry;

typedef struct Light {
    bool active;
    bool dirty;
    bool valid;

    Vector2 pos;
    RenderTexture mask;
    float outerRadius;
    Rectangle bounds;

    ShadowGeometry* shadows;
} Light;

typedef struct LighSystem {
    RenderTexture lightMask;
    Light* lights;
} LighSystem;

struct Context;
void initLightSystem(struct Context* context);

int32_t createLight(struct Context* context, Vector2 pos, float radius);
void moveLight(struct Context* context, int32_t index, Vector2 pos);


#endif
