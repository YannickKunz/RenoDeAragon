#ifndef LEVEL_H
#define LEVEL_H

#include "cloud.h"
#include "enemy.h"
#include "platform.h"
#include "raylib.h"
#include <string>
#include <tuple>
#include <vector>

struct Level {
  Vector2 spawnPoint;
  std::vector<Enemy> enemies;
  std::vector<Platform> platforms;
  std::vector<Cloud> clouds;
  Vector2 sunPosition;
  std::tuple<Texture2D, Texture2D> backgrounds;
  std::pair<std::string, std::string> musicPath;
  bool isDay;
  // Platform Textures
  Texture2D texBasic;
  Texture2D texMushroom;
  Texture2D texFlower;
  Rectangle exitZone;

  // Helper methods to make adding objects easier
  void AddCloud(float x, float y, float width, float height, float speed,
                float leftLimit, float rightLimit, bool movingRight);
  void AddPlatform(float x, float y, float width, float height,
                   PlatformType type);
  void AddEnemy(float x, float y, float width, float height, bool patrolSide,
                int patrolPlatformIndex, EnemyType type);

  void Unload(); // Cleanup textures
};

std::vector<Level> InitLevels();
Texture2D LoadAndCrop(const char *fileName);

#endif
