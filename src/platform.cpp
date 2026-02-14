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

extern bool playerToggle;

bool isPlatformActive(Platform& platform) {
	switch (platform.type) {
		case mushroom:
			return !playerToggle;
		case flower:
			return playerToggle;
		default:
			return true;
	}
}

void drawPlatform(const Platform& platform) {
  Color color;
  bool draw;
  if (platform.type == mushroom) {
    color = MAROON;
	draw = !playerToggle;
  } else if (platform.type == flower) {
    color = LIME;
	draw = playerToggle;
  } else {
    color = GRAY;
	draw = true;
  }

  if (draw) {
	  DrawRectangleRec(platform.position, color);
  }
}
