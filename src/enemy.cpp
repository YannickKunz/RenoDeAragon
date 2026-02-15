#include "enemy.h"
#include <iostream>

extern bool playerToggle;

bool isEnemyActive() {
  return !playerToggle;
}

void updateEnemy(Enemy &enemy, Rectangle platform, const float delta, Texture2D enemyTexture) {
	// 1. Patrol Logic
	int direction = 1;
	if (enemy.type == spider) {
		if ((!enemy.patrolSide && enemy.position.x <= platform.x) ||
				(enemy.patrolSide && enemy.position.x >= platform.x + platform.width)) {
			enemy.patrolSide = !enemy.patrolSide;
		}

		direction = enemy.patrolSide ? 1 : -1;
		enemy.position.x += direction * ENEMY_SPEED * delta;
	}

	// 2. Define Visual Size (Scale up the texture relative to the hitbox)
	float scaleFactor = 3.0f; // Adjust this if the roach looks too big/small
	float drawWidth = enemy.size.x * scaleFactor;
	float drawHeight = enemy.size.y * scaleFactor;



	// 3. Define Source (Whole texture)
	Rectangle sourceRect = { 0.0f, 0.0f, (float)enemyTexture.width, (float)enemyTexture.height };
	// Center the drawing rectangle over the physical position
	Rectangle destRect = {
		enemy.position.x - drawWidth / 2,     // Center X
		enemy.position.y - drawHeight + 10,   // Align Bottom (with small offset)
		drawWidth,
		drawHeight
	};

	// 4. Flip Logic (If moving left, flip width)
	sourceRect.width *= direction;

	DrawTexturePro(enemyTexture, sourceRect, destRect, {0, 0}, 0.0f, WHITE);
// Optional: Debug Box (Shows the actual hitbox)
// Rectangle hitBox = { enemy.position.x - enemy.size.x/2, enemy.position.y - enemy.size.y, enemy.size.x, enemy.size.y };
// DrawRectangleLinesEx(hitBox, 2.0f, RED);
}
