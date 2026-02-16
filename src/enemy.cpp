#include "enemy.h"
#include <iostream>

bool isEnemyActive(bool playerToggle) { return !playerToggle; }

void updateEnemy(Enemy &enemy, Rectangle platform, const float delta,
                 Texture2D enemyTexture) {
  // Initialize animation if needed
  if (enemy.framesSpeed == 0) {
    enemy.framesSpeed = 8;
    enemy.currentFrame = 0;
    enemy.framesCounter = 0;
    int numFrames = (enemy.type == spider) ? 5 : 1;
    enemy.frameRec = {0.0f, 0.0f, (float)enemyTexture.width / numFrames,
                      (float)enemyTexture.height};
  }

  // Animation Update
  if (enemy.type == spider) {
    enemy.framesCounter++;
    if (enemy.framesCounter >= (60 / enemy.framesSpeed)) {
      enemy.framesCounter = 0;
      enemy.currentFrame++;
      if (enemy.currentFrame > 4)
        enemy.currentFrame = 0;
      enemy.frameRec.x =
          (float)enemy.currentFrame * (float)enemyTexture.width / 5;
    }
  } else {
    enemy.frameRec = {0.0f, 0.0f, (float)enemyTexture.width,
                      (float)enemyTexture.height};
  }

  // Patrol Logic
  if (enemy.patrolSide) {
    enemy.position.x += ENEMY_SPEED * delta;
    if (enemy.position.x > platform.x + platform.width) {
      enemy.patrolSide = false;
    }
    // Face Left? (Depends on sprite)
    if (enemy.frameRec.width > 0)
      enemy.frameRec.width *= -1;
  } else {
    enemy.position.x -= ENEMY_SPEED * delta;
    if (enemy.position.x < platform.x) {
      enemy.patrolSide = true;
    }
    // Face Right
    if (enemy.frameRec.width < 0)
      enemy.frameRec.width *= -1;
  }

  // Draw
  float scaleFactor = 1.0f; // Reset to 1.0

  Rectangle source = enemy.frameRec;

  if (enemy.type == spider) {
    // Manual Trim (Log: y=1229, h=326)
    source.y += 1200.0f;
    source.height = 400.0f;
    scaleFactor = 1.0f;
  }
  // Roach is Auto-Cropped, so its frameRec is correct (tight).

  float drawHeight = enemy.size.y * scaleFactor;
  float drawWidth = fabs(source.width) * (drawHeight / source.height);

  // Fix Roach drawing if needed (it might be thin)
  if (enemy.type == roach) {
    drawWidth =
        fabs(enemy.frameRec.width) * (drawHeight / enemy.frameRec.height);
  }

  Rectangle destRect = {enemy.position.x - drawWidth / 2,
                        enemy.position.y - drawHeight + 10, drawWidth,
                        drawHeight};

  DrawTexturePro(enemyTexture, source, destRect, {0, 0}, 0.0f, WHITE);
}
