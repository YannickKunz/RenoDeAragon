#include "Roach.h"
#include "raylib.h"

void Roach::Draw() {
  if (textureLoaded) {
    // Scale sprite to match entity dimensions
    Rectangle source = {0, 0, (float)sprite.width, (float)sprite.height};
    Rectangle dest = {position.x, position.y, width, height};
    DrawTexturePro(sprite, source, dest, {0, 0}, 0.0f, WHITE);
  } else {
    // Draw Roach shape (e.g. Brown Rectangle)
    DrawRectangleV(position, {width, height}, BROWN);
  }
}

void Roach::Update(float dt, const Level &level) {
  // Roach is stationary.
}
