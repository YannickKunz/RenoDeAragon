#include "particles.h"
#include "raylib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#include "star_donut.h"   // our star donut demo code
#include "sunlight.h"
#include <iostream>
#include "player.h"
#include "platform.h"
#include "enemy.h"
#include <vector>

typedef enum GameScreen {
	LOGO = 0,
	TITLE,
	GAMEPLAY,
	PAUSE,
	ENDING,
	CREDIT
} GameScreen;

// --- CLOUD STRUCT ---
typedef struct Cloud {
	Rectangle rect;
	float speed;
	float leftLimit;
	float rightLimit;
	bool movingRight;
} Cloud;

struct Level {
	Vector2 spawnPoint;
	Enemy enemy;
	std::vector<Platform> platforms;
	std::vector<Cloud> clouds;
	Vector2 sunPosition;
};

// void updatePlayer(Player &player, Rectangle *platformsLevel1, int platformsLevel1Length, Enemy &enemy, const float delta, Vector2 spawnPoint) {
//   player.toggleCooldown += delta; // could this overflow?
//   if (IsKeyDown(KEY_A))
//     player.position.x -= MOVEMENT * delta;
//   if (IsKeyDown(KEY_D))
//     player.position.x += MOVEMENT * delta;
//   if (IsKeyPressed(KEY_F) && (player.toggleCooldown >= TOGGLE_DELAY_SEC)) {
//     player.toggle = !player.toggle;
//     player.toggleCooldown = 0.0f;
//   }
//   if (IsKeyDown(KEY_SPACE) && player.canJump) {
//     player.speed -= JUMP_SPEED;
//     player.canJump = false;
//   }
// 
//   // Check horizontal collisions (player sides vs platform sides)
//   for (int i = 0; i < platformsLevel1Length; i++) {
//     Rectangle plat = platformsLevel1[i];
//     float playerLeft = player.position.x - player.size.x / 2;
//     float playerRight = player.position.x + player.size.x / 2;
//     float playerTop = player.position.y - player.size.y;
//     float playerBottom = player.position.y;
// 
//     // Check if vertically overlapping
//     if (playerBottom > plat.y && playerTop < plat.y + plat.height) {
//       // Check if horizontally overlapping
//       if (playerRight > plat.x && playerLeft < plat.x + plat.width) {
//         // Push out from whichever side is closer
//         float overlapLeft = playerRight - plat.x;
//         float overlapRight = (plat.x + plat.width) - playerLeft;
//         if (overlapLeft < overlapRight) {
//           player.position.x = plat.x - player.size.x / 2; // push left
//         } else {
//           player.position.x =
//               plat.x + plat.width + player.size.x / 2; // push right
//         }
//       }
//     }
//   }

//   // Collision with enemy
//   Rectangle playerRect = {player.position.x - player.size.x / 2,
//                           player.position.y - player.size.y, player.size.x,
//                           player.size.y};
// 
//   Rectangle enemyRect = {enemy.position.x - enemy.size.x / 2,
//                          enemy.position.y - enemy.size.y, enemy.size.x,
//                          enemy.size.y};
// 
//   if (CheckCollisionRecs(playerRect, enemyRect)) {
//     player.healthPoints = player.healthPoints - 5;
//     if (player.healthPoints <= 0) {
//       // Handle player death (e.g., reset position, reduce lives, etc.)
//       player.position = spawnPoint; // Reset position for demonstration
//       player.healthPoints = 5;      // Reset health points
//     }
//   }
// 
//   if (IsKeyPressed(KEY_F) && (player.toggleCooldown >= TOGGLE_DELAY_SEC)) {
//     player.toggle = !player.toggle;
//     player.toggleCooldown = 0.0f;
//   }
//   if (IsKeyDown(KEY_SPACE) && player.canJump) {
//     player.speed -= JUMP_SPEED;
//     player.canJump = false;
//   }
// 
//   // --- Vertical collision ---
//   bool hitObstacle = false;
//   for (int i = 0; i < platformsLevel1Length; i++) {
// 	  Rectangle plat = platformsLevel1[i];
// 	  float playerLeft = player.position.x - player.size.x / 2;
// 	  float playerRight = player.position.x + player.size.x / 2;
// 	  if (playerRight >= plat.x && playerLeft <= (plat.x + plat.width)) {
// 		  // Landing on top of platform (falling down)
// 		  if (player.speed > 0 && plat.y >= player.position.y &&
// 				  plat.y <= (player.position.y + player.speed * delta)) {
// 			  hitObstacle = true;
// 			  player.speed = 0.0f;
// 			  player.position.y = plat.y;
// 			  break;
// 		  }
// 		  // Head hitting bottom of platform (jumping up)
// 		  float playerHead = player.position.y - player.size.y;
// 		  float newHead = playerHead + player.speed * delta;
// 		  float platBottom = plat.y + plat.height;
// 		  if (player.speed < 0 && newHead <= platBottom &&
// 				  playerHead >= platBottom) {
// 			  player.speed = 0.0f;
// 			  player.position.y = platBottom + player.size.y;
// 		  }
// 	  }
//   }
// 
//   if (hitObstacle) {
// 	  player.canJump = true;
//   } else {
// 	  player.position.y += player.speed * delta;
// 	  player.speed += G * delta;
// 	  player.canJump = false;
//   }
// 
//   Vector2 playerPosition = {player.position.x - player.size.x / 2,
// 	  player.position.y - player.size.y};
//   DrawRectangleV(playerPosition, player.size, BLUE);
//   // DrawCircleV(player.position, 5.0f, GOLD);
//   DrawRectangleLinesEx((Rectangle){player.position.x - player.size.x / 2,
// 		  player.position.y - player.size.y,
// 		  player.size.x, player.size.y},
// 		  2.0f, BLACK);
// }

int main() {
	// Tell the window to use vsync and work on high DPI displays
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
	lvl1.platforms = {{ {400, 600, 100, 10}, basic },
		{ {600, 500, 100, 10}, basic },
		{ {0, (float)(screenHeight - 50), (float)screenWidth, 50}, basic }};
	lvl1.enemy = {lvl1.platforms[0].position.x + lvl1.platforms[0].position.width / 2,
		lvl1.platforms[0].position.y,
		{30, 30},
		false,
		0};
	lvl1.clouds = {
		{{100, 250, 200, 40}, 150.0f, 50, 600, true},  // Low cloud
		{{700, 350, 250, 40}, 100.0f, 400, 900, false} // High cloud
	};
	lvl1.sunPosition = {500.0f, -50.0f};
	levels.push_back(lvl1);

	// LEVEL 2
	Level lvl2;
	lvl2.spawnPoint = {50, 200};
	lvl2.platforms = {{{50, 250, 100, 10}, basic},
		{ {300, 550, 100, 10}, basic},
		{ {500, 450, 100, 10}, basic},
		{ {700, 350, 100, 10}, basic},
		{ {0, (float)(screenHeight - 50), (float)screenWidth, 50}, basic }};
	lvl2.enemy = {
		{lvl2.platforms[1].position.x + lvl2.platforms[1].position.width / 2, lvl2.platforms[1].position.y},
		{30, 30},
		false,
		1};
	lvl2.clouds = {
		{{200, 150, 180, 40}, 120.0f, 100, 500, true},
		{{800, 300, 220, 40}, 90.0f, 600, 1000, false},
		{{400, 100, 150, 40}, 180.0f, 300, 800, true} // Extra cloud for Level 2
	};
	lvl2.sunPosition = {800.0f, -50.0f};
	levels.push_back(lvl2);

	// LEVEL 3
	Level lvl3;
	lvl3.spawnPoint = {100, 600};
	lvl3.platforms = { { {100, 650, 150, 10}, basic},
		{ {400, 600, 100, 10}, basic},
		{ {600, 500, 100, 10}, basic},
		{ {800, 400, 100, 10}, basic},
		{ {1000, 300, 100, 10}, basic},
		{ {0, (float)(screenHeight + 200), (float)screenWidth, 50}, basic}};
	lvl3.enemy = {
		{lvl3.platforms[2].position.x + lvl3.platforms[2].position.width / 2, lvl3.platforms[2].position.y},
		{30, 30},
		false,
		2};
	lvl3.clouds = {{{300, 200, 150, 40}, 200.0f, 200, 900, true}, // Fast cloud
		{{600, 400, 150, 40}, 200.0f, 300, 1000, false}};
	lvl3.sunPosition = {200.0f, -50.0f};
	levels.push_back(lvl3);
	// -------------------

	Player player = {0};
	player.size = {40, 80};
	player.toggleCooldown = 2.0f;
	player.healthPoints = 5;
	player.position = levels[0].spawnPoint;

	GameScreen currentScreen = GAMEPLAY;

	// --- INIT CLOUDS ---
	Cloud clouds[] = {
		{{100, 250, 200, 40}, 150.0f, 50, 600, true},  // Low cloud
		{{700, 350, 250, 40}, 100.0f, 400, 900, false} // High cloud
	};
	int cloudsLength = sizeof(clouds) / sizeof(clouds[0]);
	// -------------------

	int isPlayerBurning = false;

	// Particles Setup
	CircularBuffer particleSystem = InitParticles();

	// --- SETUP STAR DONUT ---
	StarDonutState donutState;
	InitStarDonut(&donutState, screenWidth, screenHeight);

	RenderTexture2D lightLayer = LoadRenderTexture(screenWidth, screenHeight);

	int currentLevel = 1; // Start at Level 1 (human-readable, 1-based)
	bool exitGame = false;

	int titleMenuOption = 0;
	int pauseMenuOption = 0;
	int framesCounter = 0;

	while (!WindowShouldClose() && !exitGame) {
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

							   // --- 4. SUNLIGHT RAYCASTING ---
							   Vector2 sunPos = {500.0f, -50.0f};
							   //DrawCircleV(sunPos, 40, YELLOW); // Draw Sun

							   // Prepare 3D boxes
							   BoundingBox playerBox3D = {
								   (Vector3){player.position.x - player.size.x / 2,
									   player.position.y - player.size.y, -10},
								   (Vector3){player.position.x + player.size.x / 2, player.position.y,
									   10}};

							   bool anyRayHitPlayer = false;

							   // BEGIN DRAWING TO LIGHT TEXTURE
							   BeginTextureMode(lightLayer);
							   ClearBackground(BLANK); // Clear with transparency

							   // Cast rays
							   for (int x = -200; x <= screenWidth + 200; x += 40) {
								   Vector2 targetPos = {(float)x, (float)screenHeight};
								   Vector3 sunOrigin3D = {sunPos.x, sunPos.y, 0};
								   Vector3 rayDir = {targetPos.x - sunPos.x, targetPos.y - sunPos.y, 0};
								   float len = sqrt(rayDir.x * rayDir.x + rayDir.y * rayDir.y);
								   if (len == 0)
									   continue;
								   rayDir.x /= len;
								   rayDir.y /= len;
								   Ray currentRay = {sunOrigin3D, rayDir};

								   float nearestDist = 2000.0f;
								   bool hitPlayer = false;
								   Color rayColor = YELLOW; 

								   // A. Check Player
								   RayCollision playerHit = GetRayCollisionBox(currentRay, playerBox3D);
								   if (playerHit.hit) {
									   if (playerHit.distance < nearestDist) {
										   nearestDist = playerHit.distance;
										   hitPlayer = true;
									   }
								   }

								   // B. Check Static Platforms
								   for (int i = 0; i < (int)currentLvlData.platforms.size(); i++) {
									   BoundingBox platBox = {
										   (Vector3){currentLvlData.platforms[i].position.x,
											   currentLvlData.platforms[i].position.y, -10},
										   (Vector3){currentLvlData.platforms[i].position.x +
											   currentLvlData.platforms[i].position.width,
											   currentLvlData.platforms[i].position.y +
												   currentLvlData.platforms[i].position.height,
											   10}};
									   RayCollision platHit = GetRayCollisionBox(currentRay, platBox);
									   if (platHit.hit) {
										   if (platHit.distance < nearestDist) {
											   nearestDist = platHit.distance;
											   hitPlayer = false; // Shadow
										   }
									   }
								   }

								   // C. Check Clouds (Shadows)
								   for (int i = 0; i < cloudsLength; i++) {
									   BoundingBox cloudBox = {
										   (Vector3){clouds[i].rect.x, clouds[i].rect.y, -10},
										   (Vector3){clouds[i].rect.x + clouds[i].rect.width,
											   clouds[i].rect.y + clouds[i].rect.height, 10}};
									   RayCollision cloudHit = GetRayCollisionBox(currentRay, cloudBox);
									   if (cloudHit.hit) {
										   if (cloudHit.distance < nearestDist) {
											   nearestDist = cloudHit.distance;
											   hitPlayer = false; // Cloud blocked the light
										   }
									   }
								   }

								   // Draw SOLID Ray onto the texture
								   Vector2 endPos = {sunPos.x + rayDir.x * nearestDist,
									   sunPos.y + rayDir.y * nearestDist};

								   if (hitPlayer) {
									   rayColor = RED; // Solid Red
									   anyRayHitPlayer = true;
								   }

								   DrawLineEx(sunPos, endPos, 40.0f, rayColor);
								   if (hitPlayer) DrawCircleV(endPos, 5, RED);
							   }
							   EndTextureMode(); // STOP DRAWING TO TEXTURE

							   // NOW DRAW THE WHOLE LIGHT LAYER WITH TRANSPARENCY
							   // Use BLEND_ADDITIVE for a "glowing" look, or BLEND_ALPHA for standard transparency
							   // BeginBlendMode(BLEND_ADDITIVE); 
							   DrawTextureRec(lightLayer.texture, 
									   (Rectangle){0, 0, (float)screenWidth, (float)-screenHeight}, 
									   (Vector2){0, 0}, 
									   Fade(WHITE, 0.2f)); // Apply opacity here!
														   // EndBlendMode();

														   // DRAW PLATFORMS
							   for (int i = 0; i < (int)currentLvlData.platforms.size(); i++) {
								   DrawRectangleRec(currentLvlData.platforms[i].position, GRAY);
							   }


							   // --- 1. UPDATE & DRAW CLOUDS ---
							   for (int i = 0; i < cloudsLength; i++) {
								   // Move Cloud
								   if (clouds[i].movingRight) {
									   clouds[i].rect.x += clouds[i].speed * GetFrameTime();
									   if (clouds[i].rect.x > clouds[i].rightLimit)
										   clouds[i].movingRight = false;
								   } else {
									   clouds[i].rect.x -= clouds[i].speed * GetFrameTime();
									   if (clouds[i].rect.x < clouds[i].leftLimit)
										   clouds[i].movingRight = true;
								   }

								   // Draw Cloud
								   DrawRectangleRec(clouds[i].rect, Fade(SKYBLUE, 0.9f));
								   DrawRectangleLinesEx(clouds[i].rect, 2, WHITE);
							   }
							   // -------------------------------

							   // 2. CLOUD RIDING LOGIC
							   // We check if player is falling (speed > 0) or standing, and if they hit
							   // a cloud
							   if (player.speed >= 0) {
								   for (int i = 0; i < cloudsLength; i++) {
									   Rectangle plat = clouds[i].rect;
									   // Check if player is within horizontal bounds of cloud
									   if (plat.x <= player.position.x &&
											   (plat.x + plat.width) >= player.position.x) {
										   // Check vertical collision (landing on top)
										   // We use a small threshold because updatePlayer might have just
										   // moved us
										   if (player.position.y >= plat.y &&
												   player.position.y <= (plat.y + 10.0f)) {

											   player.speed = 0.0f;
											   player.position.y = plat.y;
											   player.canJump = true;

											   // Move Player with Cloud
											   float moveAmount = clouds[i].speed * GetFrameTime();
											   if (clouds[i].movingRight)
												   player.position.x += moveAmount;
											   else
												   player.position.x -= moveAmount;

											   break;
										   }
									   }
								   }
							   }

							   // UPDATE/DRAW ENEMY
							   if (!currentLvlData.platforms.empty()) {
								   int pi = currentLvlData.enemy.patrolPlatformIndex;
								   updateEnemy(currentLvlData.enemy, currentLvlData.platforms[pi].position, delta);
							   }

							   DrawCircleV(sunPos, 40, YELLOW); // Draw Sun body on top

							   // --- 5. PARTICLES & LOGIC ---
							   isPlayerBurning = anyRayHitPlayer;
							   UpdateParticles(&particleSystem);

							   if (isPlayerBurning) {
								   Vector2 center = {player.position.x,
									   player.position.y - player.size.y / 2};
								   EmitParticle(&particleSystem, center, FIRE);
								   EmitParticle(&particleSystem, center, FIRE);
								   DrawText("BURNING!", screenWidth / 2, 50, 40, RED);
							   } else {
								   DrawText("SAFE", screenWidth / 2, 50, 40, GREEN);
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

		// end the frame and get ready for the next one
		EndDrawing();
	}

	UnloadRenderTexture(lightLayer);
	CloseWindow();
	return 0;
}
