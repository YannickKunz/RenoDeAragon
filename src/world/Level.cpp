#include "Level.h"
#include "../core/Constants.h"

Level::Level() {
  spawnPoint = {100, 300};
  isDay = true;
  sunPosition = {100, 100};
  exitZone = {0, 0, 0, 0};
  foregroundDay = {0};
  foregroundNight = {0};
  hasForeground = false;
  dayMusic = {0};
  nightMusic = {0};
  hasDayMusic = false;
  hasNightMusic = false;
}

void Level::Unload() {
  // Unload background textures
  UnloadTexture(std::get<0>(backgrounds));
  UnloadTexture(std::get<1>(backgrounds));

  // Unload foreground textures
  if (hasForeground) {
    if (foregroundDay.id != 0)
      UnloadTexture(foregroundDay);
    if (foregroundNight.id != 0)
      UnloadTexture(foregroundNight);
  }

  // Unload music
  if (hasDayMusic)
    UnloadMusicStream(dayMusic);
  if (hasNightMusic)
    UnloadMusicStream(nightMusic);
}

bool Level::IsEdge(Vector2 pos) const {
  // Check if there is ground strictly below 'pos'
  Vector2 checkPoint = {pos.x, pos.y + 1.0f};

  for (const auto &plat : platforms) {
    if (CheckCollisionPointRec(checkPoint, plat.rect)) {
      // Found ground
      return false;
    }
  }
  // No ground found
  return true;
}
