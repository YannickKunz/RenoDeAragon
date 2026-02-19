#pragma once
#include "entities/Enemy.h"
#include "entities/Player.h"
#include "raylib.h"
#include "world/Level.h"
#include <vector>

class Game {
public:
  Game();
  ~Game();

  void Init();
  void Update();
  void Draw();
  void Unload();

private:
  // Game State
  bool isDayTime;
  bool debugMode;     // Toggle with H
  bool isUnloaded;    // Prevent double-free
  bool sunHintShown;  // Has the sun hint been shown?
  float sunHintTimer; // Timer for showing hint text

  Player player;

  // Level Management
  std::vector<Level> levels;
  int currentLevelIndex;

  // Current Level Entities (pointers managed by Game)
  std::vector<Enemy *> currentEnemies;

  void LoadLevel(int index);
  void UnloadCurrentLevelEntities();
  void RebuildLevelGeometry(); // Rebuild platform/spawn/exit positions for
                               // current resolution

  // Screen Management
  enum GameScreen { TITLE, STORY, GAMEPLAY, SETTINGS, WIN, GAME_OVER };
  GameScreen currentScreen;
  GameScreen previousScreen; // To return from SETTINGS

  void UpdateTitle();
  void UpdateStory();
  void UpdateGameplay();
  void UpdateGameOver();
  void UpdateSettings();
  void UpdateWin();

  void DrawTitle();
  void DrawStory();
  void DrawGameplay();
  void DrawGameOver();
  void DrawSettings();
  void DrawWin();

  void ResetGame();

  // --- Settings ---
  float masterVolume;
  float musicVolume;
  float sfxVolume;
  bool isFullscreen;
  int selectedResIndex;
  int settingsSelection; // 0=resolution, 1=music vol, 2=sfx vol, 3=fullscreen,
                         // 4=back

  struct ResOption {
    int width;
    int height;
    const char *label;
  };
  static constexpr int RES_COUNT = 4;
  ResOption resOptions[RES_COUNT] = {
      {800, 600, "800x600"},
      {1024, 768, "1024x768"},
      {1280, 800, "1280x800"},
      {1920, 1080, "1920x1080"},
  };

  void ApplyVolume();
  void ApplyResolution();

  // --- Loaded Textures (owned by Game, assigned to entities) ---
  // Player textures
  Texture2D playerIdleTex;
  Texture2D playerWalkSheet;
  Texture2D playerDeathTex;

  // Enemy textures
  Texture2D roachTex;
  Texture2D spiderSheet;

  // UI/Screen textures
  Texture2D introScreenTex;
  Texture2D introImageTex;
  Texture2D gameOverScreenTex;

  // Exit zone (watering can) textures
  Texture2D waterPotDayTex;
  Texture2D waterPotNightTex;

  // Platform textures
  Texture2D flowerTex;
  Texture2D flowerAnimSheet;
  int flowerAnimFrameCount;
  int flowerAnimCurrentFrame;
  float flowerAnimTimer;
  float flowerAnimSpeed;

  Texture2D mushroomDayTex;
  Texture2D mushroomNightTex;

  Texture2D platformDayTex;
  Texture2D platformNightTex;

  // Audio
  Sound jumpSound;
  Sound walkSound;
  Sound burnSound;
  Sound deathSound;
  Sound wateringCanSound;
  Music titleMusic;
  bool titleMusicLoaded;

  // Currently playing music tracking
  Music *currentPlayingMusic;
  bool isMusicPlaying;
};
