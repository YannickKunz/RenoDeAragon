#pragma once
#include "raylib.h"

#define ENEMY_SPEED 80.0f

typedef struct Enemy {
  Vector2 position;
  Vector2 size;
  bool patrolSide;
  int patrolPlatformIndex; // which platform this enemy patrols on
} Enemy;

void updateEnemy(Enemy &enemy, Rectangle platform, const float delta);