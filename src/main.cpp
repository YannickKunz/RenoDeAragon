#include "enemy.cpp"
#include "raylib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#include "star_donut.h"   // our star donut demo code
#include <iostream>
#include <vector>

#define TOGGLE_DELAY_SEC 2.0f
#define G 800
#define JUMP_SPEED 600.0f
#define MOVEMENT 200.0f

typedef enum GameScreen {
  LOGO = 0,
  TITLE,
  GAMEPLAY,
  PAUSE,
  ENDING,
  CREDIT
} GameScreen;

typedef struct Player {
  Vector2 position;
  Vector2 size;
  float speed;
  bool canJump;
  bool toggle;
  float toggleCooldown;
  int healthPoints;
} Player;

struct Level {
  Vector2 spawnPoint;
  Enemy enemy;
  std::vector<Rectangle> platforms;
};

void updatePlayer(Player &player, Rectangle *platformsLevel1,
                  int platformsLevel1Length, Enemy &enemy, const float delta,
                  Vector2 spawnPoint) {
  player.toggleCooldown += delta; // could this overflow?
  if (IsKeyDown(KEY_A))
    player.position.x -= MOVEMENT * delta;
  if (IsKeyDown(KEY_D))
    player.position.x += MOVEMENT * delta;
  if (IsKeyPressed(KEY_F) && (player.toggleCooldown >= TOGGLE_DELAY_SEC)) {
    player.toggle = !player.toggle;
    player.toggleCooldown = 0.0f;
  }
  if (IsKeyDown(KEY_SPACE) && player.canJump) {
    player.speed -= JUMP_SPEED;
    player.canJump = false;
  }

  // Check horizontal collisions (player sides vs platform sides)
  for (int i = 0; i < platformsLevel1Length; i++) {
    Rectangle plat = platformsLevel1[i];
    float playerLeft = player.position.x - player.size.x / 2;
    float playerRight = player.position.x + player.size.x / 2;
    float playerTop = player.position.y - player.size.y;
    float playerBottom = player.position.y;

    // Check if vertically overlapping
    if (playerBottom > plat.y && playerTop < plat.y + plat.height) {
      // Check if horizontally overlapping
      if (playerRight > plat.x && playerLeft < plat.x + plat.width) {
        // Push out from whichever side is closer
        float overlapLeft = playerRight - plat.x;
        float overlapRight = (plat.x + plat.width) - playerLeft;
        if (overlapLeft < overlapRight) {
          player.position.x = plat.x - player.size.x / 2; // push left
        } else {
          player.position.x =
              plat.x + plat.width + player.size.x / 2; // push right
        }
      }
    }
  }

  // Collision with enemy
  Rectangle playerRect = {player.position.x - player.size.x / 2,
                          player.position.y - player.size.y, player.size.x,
                          player.size.y};

  Rectangle enemyRect = {enemy.position.x - enemy.size.x / 2,
                         enemy.position.y - enemy.size.y, enemy.size.x,
                         enemy.size.y};

  if (CheckCollisionRecs(playerRect, enemyRect)) {
    player.healthPoints = player.healthPoints - 5;
    if (player.healthPoints <= 0) {
      // Handle player death (e.g., reset position, reduce lives, etc.)
      player.position = spawnPoint; // Reset position for demonstration
      player.healthPoints = 5;      // Reset health points
    }
  }

  if (IsKeyPressed(KEY_F) && (player.toggleCooldown >= TOGGLE_DELAY_SEC)) {
    player.toggle = !player.toggle;
    player.toggleCooldown = 0.0f;
  }
  if (IsKeyDown(KEY_SPACE) && player.canJump) {
    player.speed -= JUMP_SPEED;
    player.canJump = false;
  }

  // --- Vertical collision ---
  bool hitObstacle = false;
  for (int i = 0; i < platformsLevel1Length; i++) {
    Rectangle plat = platformsLevel1[i];
    float playerLeft = player.position.x - player.size.x / 2;
    float playerRight = player.position.x + player.size.x / 2;
    if (playerRight >= plat.x && playerLeft <= (plat.x + plat.width)) {
      // Landing on top of platform (falling down)
      if (player.speed > 0 && plat.y >= player.position.y &&
          plat.y <= (player.position.y + player.speed * delta)) {
        hitObstacle = true;
        player.speed = 0.0f;
        player.position.y = plat.y;
        break;
      }
      // Head hitting bottom of platform (jumping up)
      float playerHead = player.position.y - player.size.y;
      float newHead = playerHead + player.speed * delta;
      float platBottom = plat.y + plat.height;
      if (player.speed < 0 && newHead <= platBottom &&
          playerHead >= platBottom) {
        player.speed = 0.0f;
        player.position.y = platBottom + player.size.y;
      }
    }
  }

  if (hitObstacle) {
    player.canJump = true;
  } else {
    player.position.y += player.speed * delta;
    player.speed += G * delta;
    player.canJump = false;
  }

  Vector2 playerPosition = {player.position.x - player.size.x / 2,
                            player.position.y - player.size.y};
  DrawRectangleV(playerPosition, player.size, BLUE);
  // DrawCircleV(player.position, 5.0f, GOLD);
  DrawRectangleLinesEx((Rectangle){player.position.x - player.size.x / 2,
                                   player.position.y - player.size.y,
                                   player.size.x, player.size.y},
                       2.0f, BLACK);
}

int main() {
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
  const int screenWidth = 1280;
  const int screenHeight = 800;
  InitWindow(screenWidth, screenHeight, "LVLUP Game Jam 2026");
  SetExitKey(KEY_NULL);

  SearchAndSetResourceDir("resources");

  // LEVEL SETUP
  std::vector<Level> levels;

  // LEVEL 1: Easy
  Level lvl1;
  lvl1.spawnPoint = {50, (float)(screenHeight - 100)};
  lvl1.platforms = {{400, 600, 100, 10},
                    {600, 500, 100, 10},
                    {0, (float)(screenHeight - 50), (float)screenWidth, 50}};
  lvl1.enemy = {lvl1.platforms[0].x + lvl1.platforms[0].width / 2,
                lvl1.platforms[0].y,
                {30, 30},
                false,
                0};
  levels.push_back(lvl1);

  // LEVEL 2
  Level lvl2;
  lvl2.spawnPoint = {50, 200};
  lvl2.platforms = {{50, 250, 100, 10},
                    {300, 550, 100, 10},
                    {500, 450, 100, 10},
                    {700, 350, 100, 10},
                    {0, (float)(screenHeight - 50), (float)screenWidth, 50}};
  lvl2.enemy = {
      {lvl2.platforms[1].x + lvl2.platforms[1].width / 2, lvl2.platforms[1].y},
      {30, 30},
      false,
      1};
  levels.push_back(lvl2);

  // LEVEL 3
  Level lvl3;
  lvl3.spawnPoint = {100, 600};
  lvl3.platforms = {{100, 650, 150, 10},
                    {400, 600, 100, 10},
                    {600, 500, 100, 10},
                    {800, 400, 100, 10},
                    {1000, 300, 100, 10},
                    {0, (float)(screenHeight + 200), (float)screenWidth, 50}};
  lvl3.enemy = {
      {lvl3.platforms[2].x + lvl3.platforms[2].width / 2, lvl3.platforms[2].y},
      {30, 30},
      false,
      2};
  levels.push_back(lvl3);
  // -------------------

  Player player = {0};
  player.size = {40, 80};
  player.toggleCooldown = 2.0f;
  player.healthPoints = 5;
  player.position = levels[0].spawnPoint;

  GameScreen currentScreen = GAMEPLAY;

  // --- SETUP STAR DONUT ---
  StarDonutState donutState;
  InitStarDonut(&donutState, screenWidth, screenHeight);

  int currentLevel = 1; // Start at Level 1 (human-readable, 1-based)
  bool exitGame = false;

  int titleMenuOption = 0;
  int pauseMenuOption = 0;
  int framesCounter = 0;

  while (!WindowShouldClose() &&
         !exitGame) { // run the loop until the user presses ESCAPE or presses
                      // the Close button on the window
                      // Update
                      //
    float delta = GetFrameTime();
    switch (currentScreen) {
    case LOGO: {
      // TODO: Update LOGO screen variables here!

      framesCounter++; // Count frames

      // Wait for 2 seconds (120 frames) before jumping to TITLE screen
      if (framesCounter > 120) {
        currentScreen = TITLE;
      }
    } break;
    case TITLE: {
      // Simple Menu Navigation
      if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        titleMenuOption++;
      if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        titleMenuOption--;

      // Clamp options (0 to 1)
      if (titleMenuOption < 0)
        titleMenuOption = 1;
      if (titleMenuOption > 1)
        titleMenuOption = 0;

      if (IsKeyPressed(KEY_ENTER)) {
        if (titleMenuOption == 0) { // PLAY
          currentLevel = 1;
          player.position = levels[0].spawnPoint;
          player.healthPoints = 5;
          currentScreen = GAMEPLAY;
        } else if (titleMenuOption == 1) { // QUIT
          exitGame = true;
        }
      }
    } break;
    case GAMEPLAY: {
      if (IsKeyPressed(KEY_ESCAPE)) {
        currentScreen = PAUSE;
        pauseMenuOption = 0;
      }

      // LOGIC FOR LEVEL TRANSITION
      if (IsKeyPressed(KEY_ENTER)) {
        currentLevel++;
        if (currentLevel > (int)levels.size()) {
          currentScreen = ENDING;
        } else {
          player.position = levels[currentLevel - 1].spawnPoint;
          player.speed = 0;
        }
      }
    } break;
    case PAUSE: {
      // Simple Pause Menu Navigation
      if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        pauseMenuOption++;
      if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        pauseMenuOption--;

      // Clamp options (0 to 1)
      if (pauseMenuOption < 0)
        pauseMenuOption = 1;
      if (pauseMenuOption > 1)
        pauseMenuOption = 0;

      if (IsKeyPressed(KEY_ENTER)) {
        if (pauseMenuOption == 0) { // RESUME
          currentScreen = GAMEPLAY;
        } else if (pauseMenuOption == 1) { // QUIT TO TITLE
          currentScreen = TITLE;
          titleMenuOption = 0;
        }
      }
      if (IsKeyPressed(KEY_ESCAPE)) // Toggle back to game
      {
        currentScreen = GAMEPLAY;
      }

    } break;
    case ENDING: {
      // TODO: Update ENDING screen variables here!

      // Press enter to return to TITLE screen
      if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        currentScreen = CREDIT;
      }
    } break;
    case CREDIT: {
      DrawText("CREDITS SCREEN", 20, 20, 40, LIGHTGRAY);
      if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        currentScreen = TITLE;
      }
    } break;
    default:
      break;
    }
    //----------------------------------------------------------------------------------
    // Drawing
    //----------------------------------------------------------------------------------
    BeginDrawing();

    // Setup the back buffer for drawing (clear color and depth buffers)
    ClearBackground(BLACK);
    switch (currentScreen) {
    case LOGO: {
      DrawStarDonut(&donutState, screenWidth, screenHeight);
      // TODO: Draw LOGO screen here!

    } break;
    case TITLE: {
      DrawRectangle(0, 0, screenWidth, screenHeight, DARKGREEN);
      DrawText("TITLE SCREEN",
               screenWidth / 2 - MeasureText("TITLE SCREEN", 30) / 2,
               screenHeight / 3, 30, YELLOW);

      // Draw Menu
      Color playColor = (titleMenuOption == 0) ? YELLOW : WHITE;
      Color quitColor = (titleMenuOption == 1) ? YELLOW : WHITE;

      DrawText("PLAY GAME", screenWidth / 2 - MeasureText("PLAY GAME", 30) / 2,
               screenHeight / 2, 30, playColor);
      DrawText("QUIT", screenWidth / 2 - MeasureText("QUIT", 30) / 2,
               screenHeight / 2 + 50, 30, quitColor);

      if (titleMenuOption == 0)
        DrawText(">", screenWidth / 2 - MeasureText("PLAY GAME", 30) / 2 - 30,
                 screenHeight / 2, 30, YELLOW);
      if (titleMenuOption == 1)
        DrawText(">", screenWidth / 2 - MeasureText("QUIT", 30) / 2 - 30,
                 screenHeight / 2 + 50, 30, YELLOW);

    } break;
    case GAMEPLAY: {
      DrawRectangle(0, 0, screenWidth, screenHeight, PURPLE);
      DrawText(TextFormat("LEVEL %d", currentLevel), 20, 20, 40, MAROON);

      // GET DATA FOR CURRENT LEVEL
      int levelIndex = currentLevel - 1;
      if (levelIndex >= (int)levels.size())
        levelIndex = 0;
      Level &currentLvlData = levels[levelIndex];

      // Debug text
      std::string debugText =
          "Debug coordinates: " + std::to_string(player.position.x) + ", " +
          std::to_string(player.position.y) +
          "\nToggle: " + std::to_string(player.toggle) +
          ", Time: " + std::to_string(player.toggleCooldown);
      DrawText(debugText.c_str(), 10, 10, 20, WHITE);

      // UPDATE PLAYER using current level data
      updatePlayer(player, currentLvlData.platforms.data(),
                   (int)currentLvlData.platforms.size(), currentLvlData.enemy,
                   delta, currentLvlData.spawnPoint);

      // DRAW PLATFORMS
      for (int i = 0; i < (int)currentLvlData.platforms.size(); i++) {
        DrawRectangleRec(currentLvlData.platforms[i], GRAY);
      }

      // UPDATE/DRAW ENEMY
      if (!currentLvlData.platforms.empty()) {
        int pi = currentLvlData.enemy.patrolPlatformIndex;
        updateEnemy(currentLvlData.enemy, currentLvlData.platforms[pi], delta);
      }

    } break;
    case PAUSE: {
      // Draw gameplay background dimmed (optional logic, hard to do with switch
      // unless we draw gameplay first) For simplicity, just a grey background
      DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));

      DrawText("PAUSED", screenWidth / 2 - MeasureText("PAUSED", 40) / 2,
               screenHeight / 4, 40, WHITE);

      // Draw Menu
      Color resumeColor = (pauseMenuOption == 0) ? YELLOW : WHITE;
      Color quitColor = (pauseMenuOption == 1) ? YELLOW : WHITE;

      DrawText("RESUME", screenWidth / 2 - MeasureText("RESUME", 30) / 2,
               screenHeight / 2, 30, resumeColor);
      DrawText("QUIT TO TITLE",
               screenWidth / 2 - MeasureText("QUIT TO TITLE", 30) / 2,
               screenHeight / 2 + 50, 30, quitColor);

      if (pauseMenuOption == 0)
        DrawText(">", screenWidth / 2 - MeasureText("RESUME", 30) / 2 - 30,
                 screenHeight / 2, 30, YELLOW);
      if (pauseMenuOption == 1)
        DrawText(">",
                 screenWidth / 2 - MeasureText("QUIT TO TITLE", 30) / 2 - 30,
                 screenHeight / 2 + 50, 30, YELLOW);

    } break;
    case ENDING: {
      // TODO: Draw ENDING screen here!
      DrawRectangle(0, 0, screenWidth, screenHeight, BLUE);
      DrawText("ENDING SCREEN",
               screenWidth / 2 - MeasureText("ENDING SCREEN", 40) / 2,
               screenHeight / 4, 40, LIGHTGRAY);
      DrawText(
          "PRESS ENTER or TAP to RETURN to TITLE SCREEN",
          screenWidth / 2 -
              MeasureText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 20) /
                  2,
          screenHeight - 100, 20, LIGHTGRAY);

    } break;
    case CREDIT: {
      DrawRectangle(0, 0, screenWidth, screenHeight, DARKGRAY);
      DrawText("CREDITS", screenWidth / 2 - MeasureText("CREDITS", 40) / 2,
               screenHeight / 4, 40, LIGHTGRAY);
      DrawText(
          "This game was developed by Reino de Aragon",
          screenWidth / 2 -
              MeasureText("This game was developed by Reino de Aragon", 20) / 2,
          screenHeight / 2, 20, LIGHTGRAY);
      DrawText(
          "PRESS ENTER or TAP to RETURN to TITLE SCREEN",
          screenWidth / 2 -
              MeasureText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 20) /
                  2,
          screenHeight - 100, 20, LIGHTGRAY);

    } break;
    default:
      break;
    }

    // end the frame and get ready for the next one  (display frame, poll input,
    // etc...)
    EndDrawing();
  }
  // destroy the window and cleanup the OpenGL context
  CloseWindow();
  return 0;
}
