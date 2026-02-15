#ifndef PLATFORM_H
#define PLATFORM_H

#include "raylib.h"
#include <iostream>

#define MUSHROOM_WIDTH 25
#define MUSHROOM_HEIGHT 50

#define FLOWER_WIDTH 25
#define FLOWER_HEIGHT 200

typedef enum PlatformType { basic, mushroom, flower, invisible } PlatformType;

typedef struct Platform {
  Rectangle position;
  PlatformType type;
} Platform;

void drawPlatform(const Platform &platform);

bool isPlatformActive(Platform &platform);

#endif
