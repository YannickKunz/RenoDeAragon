#include "enemy.cpp"
#include "raylib.h"
#include <iostream>
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "star_donut.h"	// our star donut demo code
#include "sunlight.h"
#include "particles.h"


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

// --- CLOUD STRUCT ---
typedef struct Cloud {
    Rectangle rect;
    float speed;
    float leftLimit;
    float rightLimit;
    bool movingRight;
} Cloud;

void updatePlayer(Player &player, Rectangle *platforms, int platformsLength,
                  Enemy &enemy, const float delta, Vector2 spawnPoint) {
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
  for (int i = 0; i < platformsLength; i++) {
    Rectangle plat = platforms[i];
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
  Rectangle playerRect = {
      player.position.x - player.size.x / 2,
      player.position.y - player.size.y,
      player.size.x,
      player.size.y};

  Rectangle enemyRect = {
      enemy.position.x - enemy.size.x / 2,
      enemy.position.y - enemy.size.y,
      enemy.size.x,
      enemy.size.y
    };

  if (CheckCollisionRecs(playerRect, enemyRect)) {
    player.healthPoints = player.healthPoints - 5;
    if (player.healthPoints <= 0) {
      // Handle player death (e.g., reset position, reduce lives, etc.)
      player.position = spawnPoint; // Reset position for demonstration
      player.healthPoints = 5; // Reset health points
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
  for (int i = 0; i < platformsLength; i++) {
    Rectangle plat = platforms[i];
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

int main () {
  // Tell the window to use vsync and work on high DPI displays
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
  // Create the window and OpenGL context
  const int screenWidth = 1280;
  const int screenHeight = 800;
  InitWindow(screenWidth, screenHeight, "LVLUP Game Jam 2026");
  SetExitKey(KEY_NULL);

  // Utility function from resource_dir.h to find the resources folder and set
  // it as the current working directory so we can load from it
  SearchAndSetResourceDir("resources");
  Vector2 spawnPoint = {50, screenHeight - 50};
  Player player = {0};
  player.position = spawnPoint;
  player.size = {40, 80};
  player.toggleCooldown = 2.0f;
  player.healthPoints = 5;

  Rectangle platforms[] = {
      {400, 600, 100, 10},
      {600, 500, 100, 10},
      {0, screenHeight - 50, screenWidth, 50},
  };

  int platformsLength = sizeof(platforms) / sizeof(platforms[0]);

  Enemy enemy = {{platforms[0].x, platforms[0].y}, {30, 30}};
  GameScreen currentScreen = GAMEPLAY;
    // --- INIT CLOUDS ---
    Cloud clouds[] = {
        { {100, 250, 200, 40}, 150.0f, 50, 600, true },   // Low cloud
        { {700, 350, 250, 40}, 100.0f, 400, 900, false }  // High cloud
    };
    int cloudsLength = sizeof(clouds)/sizeof(clouds[0]);
    // -------------------

    int isPlayerBurning = false;

    // Particles Setup
    CircularBuffer particleSystem = InitParticles();

  // --- SETUP STAR DONUT ---
  StarDonutState donutState; 
  InitStarDonut(&donutState, screenWidth, screenHeight);
	GameScreen currentScreen = LOGO;
  int currentLevel = 0;              // Track current level (used on GAMEPLAY screen) 
  bool exitGame = false;             // Track when the user wants to exit the game

  int titleMenuOption = 0;
  int pauseMenuOption = 0;

  int framesCounter = 0;          // Useful to count frames

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
          currentLevel = 1;         // Reset level
          currentScreen = GAMEPLAY;
        } else if (titleMenuOption == 1) { // QUIT
          exitGame = true;
        }
      }
    } break;
    case GAMEPLAY: {
      // TODO: Update GAMEPLAY screen variables here!
      if (IsKeyPressed(KEY_ESCAPE)) {
        currentScreen = PAUSE;
        pauseMenuOption = 0; // Reset pause menu option
      }
      // Press enter to change to ENDING screen
      if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        currentLevel++; // Increment level (just for demonstration, not used in
                        // this example)
        if (currentLevel > 3) {
          currentScreen = ENDING;
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
      DrawText(TextFormat("GAMEPLAY - LEVEL %d", currentLevel), 20, 20, 40,
               MAROON);
      DrawText("PRESS ENTER to WIN LEVEL / ESC to PAUSE",
               screenWidth / 2 -
                   MeasureText("PRESS ENTER to WIN LEVEL / ESC to PAUSE", 20) /
                       2,
               screenHeight - 100, 20, MAROON);
      // draw some text using the default font
      std::string debugText =
          "Debug coordinates: " + std::to_string(player.position.x) + ", " +
          std::to_string(player.position.y) +
          "\nToggle: " + std::to_string(player.toggle) +
          ", Time: " + std::to_string(player.toggleCooldown);
				} break;
			case GAMEPLAY:
				{
					DrawRectangle(0, 0, screenWidth, screenHeight, PURPLE);

                    
                    DrawText(TextFormat("GAMEPLAY - LEVEL %d", currentLevel), 20, 20, 40, MAROON);
					DrawText("PRESS ENTER to WIN LEVEL / ESC to PAUSE", screenWidth/2 - MeasureText("PRESS ENTER to WIN LEVEL / ESC to PAUSE", 20)/2, screenHeight - 100, 20, MAROON);
					// draw some text using the default font
					std::string debugText = "Debug coordinates: " + std::to_string(player.position.x) 
						+ ", " + std::to_string(player.position.y)
						+ "\nToggle: " + std::to_string(player.toggle)
						+ ", Time: " + std::to_string(player.toggleCooldown);

      DrawText(debugText.c_str(), 10, 10, 20, WHITE);

      updatePlayer(player, platforms, platformsLength, enemy, delta, spawnPoint);
      for (int i = 0; i < platformsLength; i++) {
        Rectangle rec = platforms[i];
        DrawRectangleRec(rec, GRAY);
      }

      updateEnemy(enemy, platforms[0], delta);
					DrawText(debugText.c_str(), 10, 10, 20, WHITE);
                    // --- 1. UPDATE & DRAW CLOUDS ---
                    for (int i = 0; i < cloudsLength; i++) {
                        // Move Cloud
                        if (clouds[i].movingRight) {
                            clouds[i].rect.x += clouds[i].speed * GetFrameTime();
                            if (clouds[i].rect.x > clouds[i].rightLimit) clouds[i].movingRight = false;
                        } else {
                            clouds[i].rect.x -= clouds[i].speed * GetFrameTime();
                            if (clouds[i].rect.x < clouds[i].leftLimit) clouds[i].movingRight = true;
                        }
                        
                        // Draw Cloud
                        DrawRectangleRec(clouds[i].rect, Fade(SKYBLUE, 0.9f));
                        DrawRectangleLinesEx(clouds[i].rect, 2, WHITE);
                    }
                    // -------------------------------
					updatePlayer(player, platforms, platformsLength);



                    // 2. CLOUD RIDING LOGIC (Moved to main)
                    // We check if player is falling (speed > 0) or standing, and if they hit a cloud
                    if (player.speed >= 0) {
                        for (int i = 0; i < cloudsLength; i++) {
                            Rectangle plat = clouds[i].rect;
                            // Check if player is within horizontal bounds of cloud
                            if (plat.x <= player.position.x && (plat.x + plat.width) >= player.position.x) {
                                // Check vertical collision (landing on top)
                                // We use a small threshold because updatePlayer might have just moved us
                                if (player.position.y >= plat.y && player.position.y <= (plat.y + 10.0f)) {
                                    
                                    player.speed = 0.0f;
                                    player.position.y = plat.y;
                                    player.canJump = true; 
                                    
                                    // Move Player with Cloud
                                    float moveAmount = clouds[i].speed * GetFrameTime();
                                    if (clouds[i].movingRight) player.position.x += moveAmount;
                                    else player.position.x -= moveAmount;

                                    break; 
                                }
                            }
                        }
                    }

					// --- 3. DRAW PLATFORMS ---
                    for (int i = 0; i < platformsLength; i++) {
                        DrawRectangleRec(platforms[i], GRAY);
                    }

                    // --- 4. SUNLIGHT RAYCASTING ---
                    Vector2 sunPos = { 500.0f, -50.0f }; 
                    DrawCircleV(sunPos, 40, YELLOW); // Draw Sun

                    // Prepare 3D boxes
                    BoundingBox playerBox3D = {
                        (Vector3){ player.position.x - player.size.x/2, player.position.y - player.size.y, -10 },
                        (Vector3){ player.position.x + player.size.x/2, player.position.y, 10 }
                    };

                    bool anyRayHitPlayer = false;

                    // Cast rays
                    for (int x = -200; x <= screenWidth + 200; x += 40) 
                    {
                        Vector2 targetPos = { (float)x, (float)screenHeight };
                        Vector3 sunOrigin3D = { sunPos.x, sunPos.y, 0 };
                        Vector3 rayDir = { targetPos.x - sunPos.x, targetPos.y - sunPos.y, 0 };
                        float len = sqrt(rayDir.x*rayDir.x + rayDir.y*rayDir.y);
                        if (len == 0) continue;
                        rayDir.x /= len; rayDir.y /= len;
                        Ray currentRay = { sunOrigin3D, rayDir };

                        float nearestDist = 2000.0f; 
                        bool hitPlayer = false;
                        Color rayColor = Fade(YELLOW, 0.15f);

                        // A. Check Player
                        RayCollision playerHit = GetRayCollisionBox(currentRay, playerBox3D);
                        if (playerHit.hit) {
                            if (playerHit.distance < nearestDist) {
                                nearestDist = playerHit.distance;
                                hitPlayer = true;
                            }
                        }

                        // B. Check Static Platforms
                        for (int i = 0; i < platformsLength; i++) {
                            BoundingBox platBox = {
                                (Vector3){ platforms[i].x, platforms[i].y, -10 },
                                (Vector3){ platforms[i].x + platforms[i].width, platforms[i].y + platforms[i].height, 10 }
                            };
                            RayCollision platHit = GetRayCollisionBox(currentRay, platBox);
                            if (platHit.hit) {
                                if (platHit.distance < nearestDist) {
                                    nearestDist = platHit.distance;
                                    hitPlayer = false; // Shadow
                                }
                            }
                        }

                        // C. Check Clouds (Shadows) <--- THIS WAS MISSING HERE
                        for (int i = 0; i < cloudsLength; i++) {
                            BoundingBox cloudBox = {
                                (Vector3){ clouds[i].rect.x, clouds[i].rect.y, -10 },
                                (Vector3){ clouds[i].rect.x + clouds[i].rect.width, clouds[i].rect.y + clouds[i].rect.height, 10 }
                            };
                            RayCollision cloudHit = GetRayCollisionBox(currentRay, cloudBox);
                            if (cloudHit.hit) {
                                if (cloudHit.distance < nearestDist) {
                                    nearestDist = cloudHit.distance;
                                    hitPlayer = false; // Cloud blocked the light
                                }
                            }
                        }

                        // Draw Ray
                        Vector2 endPos = { 
                            sunPos.x + rayDir.x * nearestDist, 
                            sunPos.y + rayDir.y * nearestDist 
                        };

                        if (hitPlayer) {
                            rayColor = Fade(RED, 0.6f);
                            anyRayHitPlayer = true;
                        }

                        DrawLineEx(sunPos, endPos, 30.0f, rayColor);
                        if (hitPlayer) DrawCircleV(endPos, 3, RED);
                    }
                    
                    // --- 5. PARTICLES & LOGIC ---
                    isPlayerBurning = anyRayHitPlayer;
                    UpdateParticles(&particleSystem);
                           
                    if (isPlayerBurning) {
                        Vector2 center = { player.position.x, player.position.y - player.size.y/2 };
                        EmitParticle(&particleSystem, center, FIRE);
                        EmitParticle(&particleSystem, center, FIRE);
                        DrawText("BURNING!", screenWidth/2, 50, 40, RED);
                    } else {
                        DrawText("SAFE", screenWidth/2, 50, 40, GREEN);
                    }

                    DrawParticles(&particleSystem);

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
