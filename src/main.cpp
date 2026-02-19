#include "Game.h"
#include "core/Constants.h"

int main() {
  InitWindow(Core::SCREEN_WIDTH, Core::SCREEN_HEIGHT, "Reino de Aragon");
  SetExitKey(0); // Disable ESC closing window - we handle ESC for settings menu
  SetTargetFPS(60);
  InitAudioDevice(); // Initialize audio system

  Game game;
  game.Init();

  while (!WindowShouldClose()) {
    game.Update();
    game.Draw();
  }

  game.Unload();
  CloseAudioDevice(); // Close audio system
  CloseWindow();
  return 0;
}