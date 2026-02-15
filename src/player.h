#ifndef PLAYER_H
#define PLAYER_H

#include "enemy.h"
#include "platform.h"
#include "raylib.h"

typedef struct Player {
  Vector2 position;
  Vector2 size;
  float speed;
  bool canJump;
  bool toggle;
  float toggleCooldown;
  int healthPoints;
} Player;

void updatePlayer(Player &player, Platform *platforms, int platformsLength,
                  Enemy &enemy, const float delta, Vector2 spawnPoint);

#endif
