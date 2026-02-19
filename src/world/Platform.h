#pragma once
#include "../core/Types.h"
#include "raylib.h"

struct Platform {
  Rectangle rect;
  PlatformType type;

  // Visuals
  Color color;
  Texture2D texture;
  bool useTexture;

  // Logic
  float initialY;

  // Constructor matching user's init style: {{x,y,w,h}, type}
  Platform(Rectangle r, PlatformType t = PlatformType::NORMAL)
      : rect(r), type(t), color(GRAY), useTexture(false), texture({0}),
        initialY(r.y) {
    // Default colors for types if no texture
    if (type == PlatformType::FLOWER)
      color = PINK;
    else if (type == PlatformType::MUSHROOM)
      color = GREEN;
    else if (type == PlatformType::INVISIBLE)
      color = BLANK;
  }

  // Constructor with color override
  Platform(Rectangle r, PlatformType t, Color c)
      : rect(r), type(t), color(c), useTexture(false), texture({0}),
        initialY(r.y) {}

  // Constructor with texture
  Platform(Rectangle r, PlatformType t, Texture2D tex)
      : rect(r), type(t), color(WHITE), useTexture(true), texture(tex),
        initialY(r.y) {}

  bool IsSolid(bool isDayTime) const {
    if (type == PlatformType::NORMAL)
      return true;
    if (type == PlatformType::INVISIBLE)
      return true;
    if (type == PlatformType::FLOWER && isDayTime)
      return true;
    if (type == PlatformType::MUSHROOM && !isDayTime)
      return true;
    return false;
  }

  bool IsActive() const { return true; }
};
