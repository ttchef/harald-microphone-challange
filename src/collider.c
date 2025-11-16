
#include "collider.h"
#include "game.h"
#include "context.h"

bool checkAABBPlayer(Collider* collider, Player* p) {
    return (
        p->pos.x < collider->pos.x + collider->dim.x && 
        p->pos.x + p->dim.x > collider->pos.x &&
        p->pos.y < collider->pos.y + collider->dim.y && 
        p->pos.y + p->dim.y > collider->pos.y
    );
}

