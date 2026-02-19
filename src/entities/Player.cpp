#include "Player.h"
#include "../core/Constants.h"
#include "raylib.h"

Player::Player() {
  position = {100, 300}; // Default spawn
  velocity = {0, 0};
  speed = Core::PLAYER_SPEED;
  jumpForce = Core::JUMP_FORCE;
  isGrounded = false;
  facingRight = true;
  isMoving = false;
  textureLoaded = false;
  deathSpriteLoaded = false;
  deathSprite = {0};

  maxHp = 5.0f;
  hp = maxHp;
  isDead = false;
}

void Player::TakeDamage(float amount) {
  if (isDead)
    return;
  hp -= amount;
  if (hp <= 0) {
    hp = 0;
    Die();
  }
}

void Player::Die() { isDead = true; }

void Player::Draw() {
  if (isDead && deathSpriteLoaded) {
    // Draw death sprite
    Rectangle source = {0, 0, (float)deathSprite.width,
                        (float)deathSprite.height};
    Rectangle dest = {position.x, position.y, width, height};
    DrawTexturePro(deathSprite, source, dest, {0, 0}, 0.0f, WHITE);
    return;
  }

  if (isMoving && animated && spritesheet.id != 0) {
    // Draw walking animation from spritesheet
    float frameW = (float)spritesheet.width / (float)frameCount;
    float frameH = (float)spritesheet.height;
    Rectangle source = {frameW * currentFrame, 0, frameW, frameH};
    // Flip horizontally if facing left
    if (!facingRight)
      source.width *= -1;
    Rectangle dest = {position.x, position.y, width, height};
    DrawTexturePro(spritesheet, source, dest, {0, 0}, 0.0f, WHITE);
  } else if (textureLoaded) {
    // Draw idle sprite
    Rectangle source = {0, 0, (float)sprite.width, (float)sprite.height};
    // Flip if facing left
    if (!facingRight)
      source.width *= -1;
    Rectangle dest = {position.x, position.y, width, height};
    DrawTexturePro(sprite, source, dest, {0, 0}, 0.0f, WHITE);
  } else {
    // Fallback debug draw
    DrawRectangleV(position, {width, height}, RED);
  }
}

void Player::Update(float delta, const std::vector<Platform> &platforms,
                    bool isDayTime) {
  // --- INPUT ---
  isMoving = false;
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
    velocity.x = -speed;
    facingRight = false;
    isMoving = true;
  } else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
    velocity.x = speed;
    facingRight = true;
    isMoving = true;
  } else {
    velocity.x = 0;
  }

  if ((IsKeyPressed(KEY_SPACE)) && isGrounded) {
    velocity.y = -jumpForce;
    isGrounded = false;
  }

  // Advance walk animation when moving
  if (isMoving && animated && frameCount > 1) {
    frameTimer += delta;
    if (frameTimer >= frameSpeed) {
      frameTimer -= frameSpeed;
      currentFrame = (currentFrame + 1) % frameCount;
    }
  } else {
    currentFrame = 0;
    frameTimer = 0.0f;
  }

  // --- PHYSICS: AXIS SEPARATION ---

  // 1. Horizontal Pass
  position.x += velocity.x * delta;

  // Update rect for collision check
  Rectangle playerRect = GetRect();

  for (const auto &plat : platforms) {
    if (!plat.IsSolid(isDayTime))
      continue;

    if (CheckCollisionRecs(playerRect, plat.rect)) {
      // Collision on X
      if (velocity.x > 0) {
        // Moving Right -> Hit Left Wall
        position.x = plat.rect.x - playerRect.width;
      } else if (velocity.x < 0) {
        // Moving Left -> Hit Right Wall
        position.x = plat.rect.x + plat.rect.width;
      }
      velocity.x = 0;
    }
  }

  // 2. Vertical Pass
  velocity.y += Core::GRAVITY * delta;
  position.y += velocity.y * delta;

  // Update rect again for Y check
  playerRect = GetRect();
  isGrounded = false; // Assume falling

  for (const auto &plat : platforms) {
    if (!plat.IsSolid(isDayTime))
      continue;

    if (CheckCollisionRecs(playerRect, plat.rect)) {
      // Collision on Y
      if (velocity.y > 0) {
        // Falling Down -> Hit Floor
        position.y = plat.rect.y - playerRect.height;
        velocity.y = 0;
        isGrounded = true;

        // SPECIAL LOGIC: MUSHROOM
        if (plat.type == PlatformType::MUSHROOM && !isDayTime) {
          velocity.y = -jumpForce * 1.5f;
          isGrounded = false;
        }
      } else if (velocity.y < 0) {
        // Jumping Up -> Hit Ceiling
        position.y = plat.rect.y + plat.rect.height;
        velocity.y = 0;
      }
    }
  }
}
