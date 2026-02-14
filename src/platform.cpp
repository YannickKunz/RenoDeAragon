#pragma once
#include "raylib.h"

#define MUSHROOM_WIDTH 25
#define MUSHROOM_HEIGHT 50

#define FLOWER_WIDTH 25
#define FLOWER_HEIGHT 100
typedef enum PlatformType { basic, mushroom, flower } PlatformType;

typedef struct Platform {
  Rectangle position;
  PlatformType type;
} Platform;

void drawPlatform(Platform &platform) {
  Color color;
  if (platform.type == mushroom) {
    color = MAROON;
  } else if (platform.type == flower) {
    color = LIME;
  } else {
    color = GRAY;
  }

  DrawRectangleRec(platform.position, color);
}
