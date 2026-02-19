#pragma once
#include "../world/Platform.h"
#include "Entity.h"
#include <vector>

class Player : public Entity {
public:
  Vector2 velocity;
  bool isGrounded;
  float speed;
  float jumpForce;

  // Additional state for animation or logic if needed
  bool facingRight;
  bool isMoving;

  // Health System
  float hp;
  float maxHp;
  bool isDead;

  // Extra textures
  Texture2D deathSprite;
  bool deathSpriteLoaded;

  void TakeDamage(float amount);
  void Die();

  Player();

  // Custom Update signature to include Platform collision context
  void Update(float delta, const std::vector<Platform> &platforms,
              bool isDayTime);
  void Draw() override;
};
