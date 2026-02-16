#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

#define ENEMY_SPEED 80.0f

typedef enum EnemyType { spider, roach } EnemyType;

typedef struct Enemy {
  Vector2 position;
  Vector2 size;
  bool patrolSide;
  int patrolPlatformIndex; // which platform this enemy patrols on
  // Texture2D enemyTexture;
  EnemyType type;

  // Animation fields
  Rectangle frameRec;
  int currentFrame;
  int framesCounter;
  int framesSpeed;
  Texture2D
      texture; // Helper to store specific texture if needed, or just pass it
} Enemy;

void updateEnemy(Enemy &enemy, Rectangle platform, const float delta,
                 Texture2D enemyTexture);

bool isEnemyActive(bool playerToggle);

#endif
