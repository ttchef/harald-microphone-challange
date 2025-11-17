
#include "collider.h"
#include "game.h"
#include "context.h"

bool checkAABBPlayer(Collider* collider, Player* p) {
    if (collider->type == COLLIDER_TYPE_RECTANGLE) {
        return (p->pos.x + p->hitboxOrigin.x < collider->pos.x + collider->dim.x && 
            p->pos.x + p->hitboxOrigin.x + p->hitbox.x > collider->pos.x &&
            p->pos.y + p->hitboxOrigin.y < collider->pos.y + collider->dim.y && 
            p->pos.y + p->hitboxOrigin.y + p->hitbox.y > collider->pos.y
        );
    }
    else if (collider->type == COLLIDER_TYPE_ONE_WAY_PLATFORM) {
        if (p->vel.y <= 0) return false;
        bool overlapX = (p->pos.x + p->hitboxOrigin.x + p->hitbox.x > collider->pos.x && 
            p->pos.x + p->hitboxOrigin.x < collider->pos.x + collider->dim.x);
        if (!overlapX) return false;

    }
    return false;
}

