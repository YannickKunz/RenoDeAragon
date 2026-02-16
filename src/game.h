#ifndef GAME_H
#define GAME_H

#include "level.h"
#include "particles.h"
#include "player.h"
#include "raylib.h"
#include <string>
#include <tuple>
#include <vector>

// Enum GameScreen
typedef enum GameScreen {
  LOGO = 0,
  TITLE,
  STORY,
  GAMEPLAY,
  PAUSE,
  GAME_OVER,
  ENDING,
  CREDIT
} GameScreen;

class Game {
public:
  Game();
  ~Game();

  void Run();

private:
  void Init();
  void Update(float delta);
  void Draw();
  void Cleanup();

  // Helper methods
  void ResetPlayer();
  void SelectPlayMusic();
  void ChangeMusicStreams();
  void UpdateMenu(bool isPauseMenu);
  void UpdateGameplay(float delta);
  void DrawLighting(Level &level);
  void DrawGameplay();
  void DrawMenu(const char *title, const char *opt1, const char *opt2,
                int selection);

  // Member variables
  GameScreen currentScreen;
  int currentLevelIndex;
  bool exitGame;
  int menuOption;
  int framesCounter;
  int burnTimer;

  Player player;

  Texture2D texSpider;
  Texture2D texRoach;
  Texture2D texIntroScreen;
  Texture2D texIntroImage;  // User requested "intro_image.png"
  Texture2D texDeathPlayer; // "deathPlayerPot"
  Texture2D texExit;        // Day Water Pot
  Texture2D texExitNight;   // Night Water Pote now in Level struct
  // Platform textures are now in Level struct
  // Texture2D texBasic;
  // Texture2D texMushroom;
  // Texture2D texFlower;
  Texture2D texGameOver;
  Texture2D texGameOver2;

  std::vector<Level> levels;

  CircularBuffer particleSystem;
  RenderTexture2D lightLayer;
  bool isPlayerBurning;

  std::pair<Music, Music> music;

  // Sound Effects
  Sound sndJump;
  Sound sndDeath;
  Sound sndWalk;
  Sound sndBurn;
  Sound sndWater;

  bool musicToggle;
  bool playerToggle;
};
#endif
