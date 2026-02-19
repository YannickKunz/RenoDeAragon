#pragma once
#include "../core/Constants.h"
#include "raylib.h"

class Entity {
public:
  Vector2 position;
  int hp;
  Texture2D sprite;
  bool textureLoaded;

  // Animation support (spritesheet)
  Texture2D spritesheet;
  int frameCount;
  int currentFrame;
  float frameTimer;
  float frameSpeed; // seconds per frame
  bool animated;

  float width;
  float height;

  Entity()
      : position({0, 0}), hp(1), sprite({0}), textureLoaded(false),
        spritesheet({0}), frameCount(1), currentFrame(0), frameTimer(0.0f),
        frameSpeed(0.1f), animated(false),
        width((float)Core::SCREEN_HEIGHT * 0.05f),
        height((float)Core::SCREEN_HEIGHT * 0.05f) {}

  virtual ~Entity() {}

  virtual void Update(float dt) {
    // Advance animation if animated
    if (animated && frameCount > 1) {
      frameTimer += dt;
      if (frameTimer >= frameSpeed) {
        frameTimer -= frameSpeed;
        currentFrame = (currentFrame + 1) % frameCount;
      }
    }
  }

  virtual void Draw() {
    if (animated && spritesheet.id != 0) {
      // Draw current frame from spritesheet
      float frameW = (float)spritesheet.width / (float)frameCount;
      float frameH = (float)spritesheet.height;
      Rectangle source = {frameW * currentFrame, 0, frameW, frameH};
      Rectangle dest = {position.x, position.y, width, height};
      DrawTexturePro(spritesheet, source, dest, {0, 0}, 0.0f, WHITE);
    } else if (textureLoaded) {
      // Scale sprite to match entity dimensions
      Rectangle source = {0, 0, (float)sprite.width, (float)sprite.height};
      Rectangle dest = {position.x, position.y, width, height};
      DrawTexturePro(sprite, source, dest, {0, 0}, 0.0f, WHITE);
    } else {
      // Debug Draw
      DrawRectangleV(position, {width, height}, RED);
    }
  }

  Rectangle GetRect() const { return {position.x, position.y, width, height}; }
};
