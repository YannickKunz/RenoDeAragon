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
  Rectangle frameRec;      // The current slice of the texture to draw
  int currentFrame;        // 0, 1, 2, etc.
  int framesCounter;       // Counts game frames to control speed
  int framesSpeed;         // How fast it animates (e.g., 8 frames per second)
  bool isFacingRight;
} Player;

void updatePlayer(Player &player, std::vector<Platform> &platforms, std::vector<Enemy> &enemies, const float delta, Vector2 spawnPoint);

void drawPlayer(Player &player);

#endif
