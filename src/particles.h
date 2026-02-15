#ifndef PARTICLES_H
#define PARTICLES_H

#include "raylib.h"

#define MAX_PARTICLES 1000

// Types
typedef enum ParticleType { WATER = 0, SMOKE, FIRE } ParticleType;

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
CircularBuffer InitParticles();

// Cleanup
void FreeParticles(CircularBuffer *cb);

// Logic
void EmitParticle(CircularBuffer *cb, Vector2 pos, ParticleType type);
void UpdateParticles(CircularBuffer *cb);
void DrawParticles(CircularBuffer *cb);

#endif