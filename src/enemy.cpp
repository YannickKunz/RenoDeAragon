#include "enemy.h"
#include <iostream>

#define ENEMY_SPEED 80.0f

extern bool playerToggle;

bool isEnemyActive() {
	return !playerToggle;
}

void updateEnemy(Enemy &enemy, Rectangle platform, const float delta,  Texture2D enemyTexture) {
  if ((!enemy.patrolSide && enemy.position.x <= platform.x) ||
      (enemy.patrolSide && enemy.position.x >= platform.x + platform.width)) {
    enemy.patrolSide = !enemy.patrolSide;
  }

  int direction = enemy.patrolSide ? 1 : -1;
  enemy.position.x += direction * ENEMY_SPEED * delta;

  // 1. Define Destination (Where enemy is)
  Rectangle destRect = {
      enemy.position.x - enemy.size.x / 2,
      enemy.position.y - enemy.size.y,
      enemy.size.x,
      enemy.size.y
  };

  // 2. Define Source (Whole texture)
  Rectangle sourceRect = { 0.0f, 0.0f, (float)enemyTexture.width, (float)enemyTexture.height };
  
  // 3. Draw Texture (Flip horizontally based on direction if you want)
  if (direction == -1) sourceRect.width *= -1; 

  DrawTexturePro(enemyTexture, sourceRect, destRect, {0, 0}, 0.0f, WHITE);
  
  // Optional: Debug Box
  // DrawRectangleLinesEx(destRect, 2.0f, RED);
}

