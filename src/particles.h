#ifndef PARTICLES_H
#define PARTICLES_H

#include "raylib.h"
#include <stdlib.h>
#include <math.h>

#define MAX_PARTICLES 1000

// Types
typedef enum ParticleType {
    WATER = 0,
    SMOKE,
    FIRE
} ParticleType;

typedef struct Particle {
    ParticleType type;
    Vector2 position;
    Vector2 velocity;
    float radius;
    Color color;
    float lifeTime;
    bool alive;
} Particle;

typedef struct CircularBuffer {
    int head; // Write index
    int tail; // Read index
    Particle *buffer;
} CircularBuffer;

// Initialize Buffer
CircularBuffer InitParticles() {
    Particle *particles = (Particle*)calloc(MAX_PARTICLES, sizeof(Particle));
    CircularBuffer cb = { 0, 0, particles };
    return cb;
}

// Cleanup
void FreeParticles(CircularBuffer *cb) {
    free(cb->buffer);
}

// Logic
static Particle *AddToCircularBuffer(CircularBuffer *circularBuffer)
{
    // If next head is tail, buffer is full
    if (((circularBuffer->head + 1) % MAX_PARTICLES) != circularBuffer->tail)
    {
        Particle *p = &circularBuffer->buffer[circularBuffer->head];
        circularBuffer->head = (circularBuffer->head + 1) % MAX_PARTICLES;
        return p;
    }
    return NULL;
}

void EmitParticle(CircularBuffer *cb, Vector2 pos, ParticleType type)
{
    Particle *p = AddToCircularBuffer(cb);
    if (p != NULL)
    {
        p->position = pos;
        p->alive = true;
        p->lifeTime = 0.0f;
        p->type = type;

        // Random starting parameters
        float speed = (float)(rand()%10)/5.0f;
        float direction = (float)(rand()%360);

        switch (type)
        {
            case FIRE:
                p->radius = 1.0f + (rand()%2);
                p->color = ORANGE; // Start Orange/Yellow
                p->velocity = (Vector2){ (float)(rand()%10 - 5)/40.0f, -0.3f - (float)(rand()%10)/40.0f }; // Upwards
                break;
            case SMOKE:
                p->radius = 5.0f;
                p->color = Fade(DARKGRAY, 0.5f);
                p->velocity = (Vector2){ (float)(rand()%10 - 5)/10.0f, -0.5f };
                break;
            default: break;
        }
    }
}

void UpdateParticles(CircularBuffer *cb)
{
    // Iterate from tail to head
    for (int i = cb->tail; i != cb->head; i = (i + 1) % MAX_PARTICLES)
    {
        if (!cb->buffer[i].alive) continue;

        Particle *p = &cb->buffer[i];
        p->lifeTime += GetFrameTime();

        // Specific Logic
        if (p->type == FIRE) {
            p->position.x += p->velocity.x;
            p->position.y += p->velocity.y; // Move up
            p->radius -= 0.1f; // Shrink
            
            // Color shift from Yellow -> Orange -> Red
            if (p->lifeTime > 0.05f) p->color = RED;
            if (p->lifeTime > 0.15f) p->color = Fade(MAROON, 0.5f);

            if (p->radius <= 0.2f) p->alive = false;
        }
    }

    // Cleanup dead particles at tail
    while ((cb->tail != cb->head) && !cb->buffer[cb->tail].alive) {
        cb->tail = (cb->tail + 1) % MAX_PARTICLES;
    }
}

void DrawParticles(CircularBuffer *cb)
{
    for (int i = cb->tail; i != cb->head; i = (i + 1) % MAX_PARTICLES)
    {
        if (cb->buffer[i].alive) {
             DrawCircleV(cb->buffer[i].position, cb->buffer[i].radius, cb->buffer[i].color);
        }
    }
}

#endif