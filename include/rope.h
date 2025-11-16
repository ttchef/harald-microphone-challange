
#ifndef ROPE_H
#define ROPE_H

#include <stdint.h>
#include <raylib.h>

#define ROPE_NUM_OF_CONSTRAINT_RUNS 50

typedef struct RopeSegment {
    Vector2 currentPos;
    Vector2 oldPos;
} RopeSegment;

typedef struct Rope {
    RopeSegment* segments;
    int32_t numSegments;
    float segmentLength;
    float dampingFactor;
} Rope;

void createRope(Rope* rope, int32_t numOfRopeSegments, Vector2 ropeStartPoint, float segmentLength, float dampingFactor);
void destroyRope(Rope* rope);

void applyRopeConstraints(Rope* rope);
void updateRope(Rope* rope, float dt);
void drawRope(Rope* rope);

#endif // ROPE_H
