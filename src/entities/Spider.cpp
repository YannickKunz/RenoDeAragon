#include "Spider.h"
#include "raylib.h"

void Spider::Update(float dt, const Level &level) {
  // Patrolling Logic
  // Move
  float move = speed * (movingRight ? 1.0f : -1.0f) * dt;
  position.x += move;

  // Advance animation
  if (animated && frameCount > 1) {
    frameTimer += dt;
    if (frameTimer >= frameSpeed) {
      frameTimer -= frameSpeed;
      currentFrame = (currentFrame + 1) % frameCount;
    }
  }

  // Check constraints
  // 1. Wall Collision or Screen Edge
  Rectangle myRect = GetRect();
  if (myRect.x < 0 ||
      myRect.x + myRect.width >
          level.platforms[0].rect.width) { // Assuming global width bound
    movingRight = !movingRight;
  }

  // 2. Platform Edge Detection
  // find the platform we are on
  bool onPlatform = false;
  // Look ahead point: Bottom Center + offset
  float lookAheadX =
      movingRight ? (myRect.x + myRect.width + 5) : (myRect.x - 5);
  Vector2 footPos = {lookAheadX, myRect.y + myRect.height + 2};

  for (const auto &plat : level.platforms) {
    if (CheckCollisionPointRec(footPos, plat.rect)) {
      onPlatform = true; // We have ground ahead
      break;
    }
  }

  if (!onPlatform) {
    // No ground ahead, turn around
    movingRight = !movingRight;
    // Bump back slightly to avoid getting stuck in a loop if speed is high
    position.x -= move * 2;
  }
}

void Spider::Draw() {
  if (animated && spritesheet.id != 0) {
    // Draw current frame from spritesheet
    float frameW = (float)spritesheet.width / (float)frameCount;
    float frameH = (float)spritesheet.height;
    Rectangle source = {frameW * currentFrame, 0, frameW, frameH};
    // Flip sprite if moving left
    if (!movingRight)
      source.width *= -1;
    Rectangle dest = {position.x, position.y, width, height};
    DrawTexturePro(spritesheet, source, dest, {0, 0}, 0.0f, WHITE);
  } else if (textureLoaded) {
    // Scale sprite to match entity dimensions
    Rectangle source = {0, 0, (float)sprite.width, (float)sprite.height};
    if (!movingRight)
      source.width *= -1;
    Rectangle dest = {position.x, position.y, width, height};
    DrawTexturePro(sprite, source, dest, {0, 0}, 0.0f, WHITE);
  } else {
    // Draw Spider shape (e.g. Purple Rectangle)
    DrawRectangleV(position, {width, height}, PURPLE);
    // Draw eyes to show direction
    float eyeX = movingRight ? (position.x + width - 10) : (position.x + 5);
    DrawRectangle(eyeX, position.y + 10, 5, 5, RED);
  }
}
