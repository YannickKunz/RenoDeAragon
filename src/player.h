#ifndef PLAYER_H
#define PLAYER_H

#include "enemy.h"
#include "platform.h"
#include "raylib.h"
#include <iostream>
#include <vector>


typedef struct Player {
  Vector2 position;
  Vector2 size;
  float speed;
  bool canJump;
  bool toggle;
  float toggleCooldown;
  int healthPoints;
  Texture2D texture;
  bool isFacingRight;
} Player;

void updatePlayer(Player &player, std::vector<Platform> &platforms, std::vector<Enemy> &enemies, const float delta, Vector2 spawnPoint);

void drawPlayer(Player &player);

#endif
