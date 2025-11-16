
#include "rope.h"
#include "game.h"

#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>

void createRope(Rope* rope, int32_t numOfRopeSegments, Vector2 ropeStartPoint, float segmentLength, float dampingFactor) {
    if (!rope) return;

    rope->numSegments = numOfRopeSegments;
    rope->segmentLength = segmentLength;
    rope->dampingFactor = dampingFactor;
    rope->segments = malloc(sizeof(RopeSegment) * numOfRopeSegments);
    if (!rope->segments) {
        fprintf(stderr, "Failed to allocate rope segments!\n");
        return;
    }

    Vector2 start = ropeStartPoint;
    for (int32_t i = 0; i < numOfRopeSegments; i++) {
        rope->segments[i] = (RopeSegment){ropeStartPoint, start};
        start.y += segmentLength;
    }
}

void destroyRope(Rope* rope) {
    if (rope && rope->segments) free(rope->segments);
}

void applyRopeConstraints(Rope *rope) {
    rope->segments[0].currentPos = GetMousePosition();
    for (int32_t i = 0; i < rope->numSegments - 1; i++) {
        RopeSegment* currentSeg = &rope->segments[i];
        RopeSegment* nextSeg = &rope->segments[i + 1];
        
        float dist = Vector2Length(Vector2Subtract(currentSeg->currentPos, nextSeg->currentPos));
        float delta = dist - rope->segmentLength;
    
        Vector2 changeDir = Vector2Normalize(Vector2Subtract(currentSeg->currentPos, nextSeg->currentPos));
        Vector2 changeVec = Vector2Scale(changeDir, delta);

        if (i != 0) {
            currentSeg->currentPos = Vector2Subtract(currentSeg->currentPos, Vector2Scale(changeVec, 0.5f));
            nextSeg->currentPos = Vector2Add(nextSeg->currentPos, Vector2Scale(changeVec, 0.5f));
        }
        else {
            nextSeg->currentPos = Vector2Add(nextSeg->currentPos, changeVec);
        }
    }
}

void updateRope(Rope *rope, float dt) {
    for (int32_t i = 0; i < rope->numSegments; i++) {
        RopeSegment* seg = &rope->segments[i];
        Vector2 velocity = Vector2Scale(Vector2Subtract(seg->currentPos, seg->oldPos), rope->dampingFactor);
        seg->oldPos = seg->currentPos;
        seg->currentPos = Vector2Add(seg->currentPos, velocity);
        seg->currentPos.y += Vector2Scale(seg->currentPos, 10 * dt * dt).y;
    }

    for (int32_t i = 0; i < ROPE_NUM_OF_CONSTRAINT_RUNS; i++) {
        applyRopeConstraints(rope);
    }
}

void drawRope(Rope* rope) {
    for (int32_t i = 0; i < rope->numSegments - 1; i++) {
        DrawLineEx(rope->segments[i].currentPos, rope->segments[i + 1].currentPos, 5.0f, RAYWHITE);
    }
}

