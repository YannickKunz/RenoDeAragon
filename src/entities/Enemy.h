#pragma once
#include "../core/Constants.h"
#include "../world/Level.h"
#include "Entity.h"

class Enemy : public Entity {
public:
  float speed;
  bool movingRight;

  Enemy(Vector2 pos) : Entity() {
    position = pos;
    speed = Core::ENEMY_SPEED;
    movingRight = true;
  }

  virtual void Update(float dt, const Level &level) {
    // Generic Patrol Logic
    float move = speed * dt;

    // Calculate proposed move
    float nextX = position.x + (movingRight ? move : -move);

    // Edge Detection
    // Need dimensions. Using GetRect()
    Rectangle rect = GetRect();

    // Check point: Bottom-Right if moving right, Bottom-Left if moving left
    Vector2 checkPos;
    checkPos.x = movingRight ? (position.x + rect.width) : position.x;
    checkPos.y = position.y + rect.height; // Bottom

    // We also want to check if the NEXT position will be off-edge?
    // Actually, checkPos should be based on current pos to see if we are AT
    // edge. But better to look ahead. Let's use the plan's simplified logic:
    // move then check.

    if (movingRight)
      position.x += move;
    else
      position.x -= move;

    // Update rect after move
    rect = GetRect();
    checkPos.x = movingRight ? (position.x + rect.width) : position.x;
    checkPos.y =
        position.y + rect.height; // Check strictly below the outer edge

    // Level::IsEdge checks if there is ground at (x, y+1)
    // We want to pass the bottom corner.
    if (level.IsEdge({checkPos.x, position.y + rect.height})) {
      // Turn around
      movingRight = !movingRight;
    }
  }

  virtual void Draw() override {
    Entity::Draw(); // Default draw
  }
};
