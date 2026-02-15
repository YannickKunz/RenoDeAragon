#include "enemy.h"
#include <iostream>

#define ENEMY_SPEED 80.0f

extern bool playerToggle;

bool isEnemyActive() {
	return !playerToggle;
}

void updateEnemy(Enemy &enemy, Rectangle platform, const float delta) {
  if ((!enemy.patrolSide && enemy.position.x <= platform.x) ||
      (enemy.patrolSide && enemy.position.x >= platform.x + platform.width)) {
    enemy.patrolSide = !enemy.patrolSide;
  }

  int direction = enemy.patrolSide ? 1 : -1;
  enemy.position.x += direction * ENEMY_SPEED * delta;

  Vector2 enemyPosition = {enemy.position.x - enemy.size.x / 2,
                           enemy.position.y - enemy.size.y};
  DrawRectangleV(enemyPosition, enemy.size, RED);
}
