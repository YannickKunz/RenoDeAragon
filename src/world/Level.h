#pragma once
#include "Platform.h"
#include "raylib.h"
#include <string>
#include <tuple>
#include <vector>

// Define Enemy Config Struct for Data-Driven Level Loading
enum class EnemyType { SPIDER, ROACH };

#define roach EnemyType::ROACH
#define spider EnemyType::SPIDER

// Using structs compatible with standard layout
struct EnemyConfig {
  Vector2 position;
  Vector2 patrolRange;
  bool movingRight;
  int platformIndex;
  EnemyType type;
};

class Level {
public:
  std::vector<Platform> platforms;
  std::vector<EnemyConfig> enemies;

  // Backgrounds (Day, Night)
  std::tuple<Texture2D, Texture2D> backgrounds;

  // Foreground layers (Day, Night) - drawn on top of gameplay
  Texture2D foregroundDay;
  Texture2D foregroundNight;
  bool hasForeground;

  Vector2 spawnPoint;
  Vector2 sunPosition;
  Rectangle exitZone;
  bool isDay;

  // Music
  Music dayMusic;
  Music nightMusic;
  bool hasDayMusic;
  bool hasNightMusic;
  std::string dayMusicPath;
  std::string nightMusicPath;

  Level();
  void Unload();
  bool IsEdge(Vector2 pos) const;
};
