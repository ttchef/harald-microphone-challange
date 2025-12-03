
#include "collider.h"
#include "game.h"
#include "context.h"

bool checkAABBPlayer(Collider* collider, Player* p) {
    float px = p->pos.x + p->hitboxOrigin.x;
    float py = p->pos.y + p->hitboxOrigin.y;
    float pw = p->hitbox.x;
    float ph = p->hitbox.y;

    float cx = collider->pos.x;
    float cy = collider->pos.y;
    float cw = collider->dim.x;
    float ch = collider->dim.y;

    if (collider->type == COLLIDER_TYPE_RECTANGLE) {
        return (px < cx + cw && px + pw > cx &&
                py < cy + ch && py + ph > cy);
    }
    else if (collider->type == COLLIDER_TYPE_ONE_WAY_PLATFORM) {
        float playerBottom = py + ph;
        float playerTop    = py;

        if (p->vel.y <= 0) return false;

        bool xOverlap = (px + pw > cx && px < cx + cw);
        if (!xOverlap) return false;

        bool wasAbove = (p->oldPos.y + p->hitboxOrigin.y + ph) <= cy;
        bool nowBelow = playerBottom >= cy;

        return wasAbove && nowBelow;

    }
    return false;
}

bool checkAABB(ColliderRect* a, ColliderRect* b) {
    return (
        a->pos.x < b->pos.x + b->dim.x && a->pos.x + a->dim.x > b->pos.x &&
        a->pos.y < b->pos.y + b->dim.y && a->pos.y + a->dim.y > b->pos.y
    );
}

