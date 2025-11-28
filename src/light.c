
#include "light.h"
#include "context.h"

#include <raylib.h>
#include <rlgl.h>

#define RLGL_SRC_ALPHA  0x0302
#define RLGL_MIN        0x8007
#define RLGL_MAX        0x8008

void initLightSystem(struct Context *context) {
    LighSystem* lightSystem = &context->gameData.lightSystem;
    lightSystem->lights = darrayCreate(Light);
    lightSystem->lightMask = LoadRenderTexture(context->gameWidth, context->windowHeight);
}

void moveLight(struct Context* context, int index, Vector2 pos) {
    LighSystem* lightSystem = &context->gameData.lightSystem;
    Light* light = &lightSystem->lights[index];

    light->dirty = true;
    light->pos = pos;
    light->bounds.x = pos.x - light->bounds.x;
    light->bounds.y = pos.y - light->bounds.y;
}

static void computeShadowVolumeForEdge(Context* context, int32_t index, Vector2 sp, Vector2 ep) {
    LighSystem* lightSystem = &context->gameData.lightSystem;
    Light* light = &lightSystem->lights[index];
    
    float extension = light->outerRadius * 2;
    
}

static void drawLightMask(Context* context, int32_t index) {
    LighSystem* lightSystem = &context->gameData.lightSystem;
    Light* light = &lightSystem->lights[index];

    BeginTextureMode(light->mask);
        ClearBackground(WHITE);
        
        rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MIN);
        rlSetBlendMode(BLEND_CUSTOM);

        // Light Radius
        if (light->valid) {
            DrawCircleGradient(
                light->pos.x, light->pos.y, light->outerRadius,
                ColorAlpha(WHITE, 0), WHITE
            );
        }

        rlDrawRenderBatchActive();
        
        rlSetBlendMode(BLEND_ALPHA);
        rlSetBlendFactors(RLGL_SRC_ALPHA, RLGL_SRC_ALPHA, RLGL_MAX);
        rlSetBlendMode(BLEND_CUSTOM);
        
        // Draw Shadows
        for (int32_t i = 0; i < darrayLength(light->shadows); i++) {
            DrawTriangleFan(light->shadows[i].vertices, 4, WHITE);
        }
        rlSetBlendMode(BLEND_ALPHA);
    EndTextureMode();
}

int32_t createLight(struct Context *context, Vector2 pos, float radius) {
    LighSystem* lightSystem = &context->gameData.lightSystem;
    
    Light light = {
        .active = true,
        .valid = false,
        .mask = LoadRenderTexture(context->gameWidth, context->windowHeight),
        .outerRadius = radius,

        .bounds.width = radius * 2,
        .bounds.height = radius * 2,
    };

    light.shadows = darrayCreate(ShadowGeometry);

    int32_t index = darrayLength(lightSystem->lights);
    darrayPush(lightSystem->lights, light);

    moveLight(context, index, pos);
    
    return index;
}

