#include "platform.h"
#include <iostream>

bool isPlatformActive(const Platform &platform, bool playerToggle) {
  switch (platform.type) {
  case mushroom:
    return !playerToggle;
  case flower:
    return playerToggle;
  case invisible:
    return true; // Physical but unseen
  default:
    return true;
  }
}

void drawPlatform(const Platform &platform, Texture2D mushroomTex,
                  Texture2D flowerTex, Texture2D basicTex, bool playerToggle) {
  Texture2D texture = {0};
  Color color = GRAY;
  Rectangle source = {0, 0, 0, 0};
  float visualScale = 1.0f; // Scale factor for padding
  float yOffset = 0.0f;     // Offset to grow upwards

  // Logic to determine texture and frame
  if (platform.type == mushroom) {
    texture = mushroomTex;
    color = MAROON;

    // Manual Trim based on logs: Content is ~600px tall around y=550
    float frameW = (float)texture.width / 2;
    float trimY = 550.0f;
    float trimHeight = 600.0f;

    source = {0, trimY, frameW, trimHeight};
    visualScale = 1.6f; // User said 2.0f was too big

    if (!playerToggle) {
      source.x = 0; // Active (Night)
    } else {
      source.x = frameW; // Inactive (Day)
    }
  } else if (platform.type == flower) {
    texture = flowerTex;
    color = LIME;

    // Manual Trim (Similar to mushroom)
    float frameW = (float)texture.width / 2;
    float trimY = 550.0f;
    float trimHeight = 600.0f;

    source = {0, trimY, frameW, trimHeight};
    visualScale = 2.0f; // Increase scale to 2.0f for visibility

    if (playerToggle) {
      source.x = 0; // Active (Day)
    } else {
      source.x = frameW; // Inactive (Night)
    }
  } else if (platform.type == basic) {
    texture = basicTex;
    color = GRAY;
    source = {0, 0, (float)texture.width, (float)texture.height};
    visualScale = 1.0f; // Basic matches hitbox
  } else {              // Invisible / fallback
    // Don't draw
    return;
  }

  if (texture.id > 0) {
    Rectangle dest = platform.position;

    // Apply Visual Scale
    if (visualScale > 1.0f) {
      float oldW = dest.width;
      float oldH = dest.height;
      dest.width *= visualScale;
      dest.height *= visualScale;

      // Center horizontally
      dest.x -= (dest.width - oldW) / 2.0f;
      // ALIGN BOTTOM (Grow upwards)
      dest.y -= (dest.height - oldH);
    }

    DrawTexturePro(texture, source, dest, {0, 0}, 0.0f, WHITE);

    // Debug
    DrawRectangleLinesEx(platform.position, 2.0f, RED);
  } else {
    // Fallback
    if (platform.type == basic) {
      DrawRectangleRec(platform.position, Fade(GRAY, 0.5f));
      DrawRectangleLinesEx(platform.position, 2.0f, RED);
    } else {
      DrawRectangleRec(platform.position, color);
    }
  }
}
