#pragma once

namespace Core {
// Screen dimensions (mutable for resolution changes)
inline int SCREEN_WIDTH = 1280;
inline int SCREEN_HEIGHT = 800;

// Physics constants (recalculated from SCREEN_HEIGHT)
inline float GRAVITY = SCREEN_HEIGHT * 1.25f;      // ~1000 at 800p
inline float JUMP_FORCE = SCREEN_HEIGHT * 0.75f;   // ~600 at 800p
inline float PLAYER_SPEED = SCREEN_HEIGHT * 0.45f; // ~360 at 800p
inline float ENEMY_SPEED = SCREEN_HEIGHT * 0.20f;  // ~160 at 800p

// Call after changing SCREEN_WIDTH/SCREEN_HEIGHT
inline void RecalculatePhysics() {
  GRAVITY = SCREEN_HEIGHT * 1.25f;
  JUMP_FORCE = SCREEN_HEIGHT * 0.75f;
  PLAYER_SPEED = SCREEN_HEIGHT * 0.45f;
  ENEMY_SPEED = SCREEN_HEIGHT * 0.20f;
}
} // namespace Core
