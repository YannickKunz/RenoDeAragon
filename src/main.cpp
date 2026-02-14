#include "raylib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#include "star_donut.h"   // our star donut demo code
#include <iostream>
#include "player.h"
#include "platform.h"
#include "enemy.h"

typedef enum GameScreen {
	LOGO = 0,
	TITLE,
	GAMEPLAY,
	PAUSE,
	ENDING,
	CREDIT
} GameScreen;

int currentLevel = 0;
bool exitGame = false; // Track when the user wants to exit the game
int titleMenuOption = 0;
int pauseMenuOption = 0;

int main() {
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	// Create the window and OpenGL context
	const int screenWidth = 1280;
	const int screenHeight = 800;
	InitWindow(screenWidth, screenHeight, "LVLUP Game Jam 2026");
	SetExitKey(KEY_NULL);

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	Player player = { 0 };
	player.position = { 50, screenHeight - 100 };
	player.size = { 50, 100 };
	player.toggleCooldown = 2.0f;
	player.healthPoints = 10;

	Platform platforms[] = {
		{ {400, 600, 100, 10}, basic},
		{ {600, 500, 100, 10}, basic},
		{ {0, screenHeight - 50, screenWidth, 50}, basic},
		{ {screenWidth - 200, screenHeight - 100, MUSHROOM_WIDTH, MUSHROOM_HEIGHT}, mushroom},
		{ {screenWidth - 400, screenHeight - 150, FLOWER_WIDTH, FLOWER_HEIGHT}, flower},
	};

	int platformsLength = sizeof(platforms)/sizeof(platforms[0]);

	Enemy enemy = { {platforms[0].position.x, platforms[0].position.y}, {30, 30} };
	GameScreen currentScreen = GAMEPLAY;

	// --- SETUP STAR DONUT ---
	StarDonutState donutState;
	InitStarDonut(&donutState, screenWidth, screenHeight);


	int currentLevel = 0;  // Track current level (used on GAMEPLAY screen)
	bool exitGame = false; // Track when the user wants to exit the game

	int titleMenuOption = 0;
	int pauseMenuOption = 0;

	int framesCounter = 0; // Useful to count frames

	while (!WindowShouldClose() && !exitGame) { // run the loop until the user presses ESCAPE or presses
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

							   DrawText(debugText.c_str(), 10, 10, 20, WHITE);

							   updatePlayer(player, platforms, platformsLength, delta);
							   for (int i = 0; i < platformsLength; i++) {
								   drawPlatform(platforms[i]);
							   }

							   updateEnemy(enemy, platforms[0].position, delta);

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
