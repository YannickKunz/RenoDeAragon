/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "star_donut.h"	// our star donut demo code

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, PAUSE, ENDING, CREDIT } GameScreen;

int main ()
{
	const int screenWidth = 1280;
    const int screenHeight = 800;

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	// Create the window and OpenGL context
	InitWindow(screenWidth, screenHeight, "Hello Raylib");
    SetExitKey(KEY_NULL);
	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");

	// --- SETUP STAR DONUT ---
    StarDonutState donutState; 
    InitStarDonut(&donutState, screenWidth, screenHeight);
    // ------------------------

	GameScreen currentScreen = LOGO;
    int currentLevel = 0;              // Track current level (used on GAMEPLAY screen) 
    bool exitGame = false;              // Track when the user wants to exit the game

    int titleMenuOption = 0;
    int pauseMenuOption = 0;

	int framesCounter = 0;          // Useful to count frames
    SetTargetFPS(60); 
	
	// game loop
	while (!WindowShouldClose() && !exitGame)		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		// Update
        //----------------------------------------------------------------------------------
        switch (currentScreen)
        {
            case LOGO:
            {
                // TODO: Update LOGO screen variables here!

                framesCounter++;    // Count frames

                // Wait for 2 seconds (120 frames) before jumping to TITLE screen
                if (framesCounter > 120)
                {
                    currentScreen = TITLE;
                }
            } break;
            case TITLE:
            {
                // Simple Menu Navigation
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) titleMenuOption++;
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) titleMenuOption--;
                
                // Clamp options (0 to 1)
                if (titleMenuOption < 0) titleMenuOption = 1;
                if (titleMenuOption > 1) titleMenuOption = 0;

                if (IsKeyPressed(KEY_ENTER))
                {
                    if (titleMenuOption == 0) // PLAY
                    {
                        currentLevel = 1; // Reset level
                        currentScreen = GAMEPLAY;
                    }
                    else if (titleMenuOption == 1) // QUIT
                    {
                        exitGame = true;
                    }
                }
            } break;
            case GAMEPLAY:
            {
                // TODO: Update GAMEPLAY screen variables here!
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    currentScreen = PAUSE;
                    pauseMenuOption = 0; // Reset pause menu option
                }
                // Press enter to change to ENDING screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentLevel++; // Increment level (just for demonstration, not used in this example)
                    if (currentLevel > 3) 
                    {
                        currentScreen = ENDING;
                    }
                }
                
            } break;
            case PAUSE:
            {
                // Simple Pause Menu Navigation
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) pauseMenuOption++;
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) pauseMenuOption--;

                // Clamp options (0 to 1)
                if (pauseMenuOption < 0) pauseMenuOption = 1;
                if (pauseMenuOption > 1) pauseMenuOption = 0;

                if (IsKeyPressed(KEY_ENTER))
                {
                    if (pauseMenuOption == 0) // RESUME
                    {
                        currentScreen = GAMEPLAY;
                    }
                    else if (pauseMenuOption == 1) // QUIT TO TITLE
                    {
                        currentScreen = TITLE;
                        titleMenuOption = 0;
                    }
                }
                if (IsKeyPressed(KEY_ESCAPE)) // Toggle back to game
                {
                     currentScreen = GAMEPLAY;
                }

            } break;
            case ENDING:
            {
                // TODO: Update ENDING screen variables here!

                // Press enter to return to TITLE screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = CREDIT;
                }
            } break;
            case CREDIT:
            {
                DrawText("CREDITS SCREEN", 20, 20, 40, LIGHTGRAY);
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                    {
                        currentScreen = TITLE;
                    }
            } break;
            default: break;
        }
		//----------------------------------------------------------------------------------
		// Drawing
		//----------------------------------------------------------------------------------
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);
		switch(currentScreen)
            {
                case LOGO:
                {
					// --- DRAW DONUT ---
                    DrawStarDonut(&donutState, screenWidth, screenHeight);
                    // ------------------
                    // TODO: Draw LOGO screen here!
                    //DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);
                    //DrawText("WAIT for 2 SECONDS...", 290, 220, 20, GRAY);

                } break;
                case TITLE:
                {
                    DrawRectangle(0, 0, screenWidth, screenHeight, DARKGREEN);
                    DrawText("TITLE SCREEN", screenWidth/2 - MeasureText("TITLE SCREEN", 30)/2, screenHeight/3, 30, YELLOW);
                    
                    // Draw Menu
                    Color playColor = (titleMenuOption == 0) ? YELLOW : WHITE;
                    Color quitColor = (titleMenuOption == 1) ? YELLOW : WHITE;

                    DrawText("PLAY GAME", screenWidth/2 - MeasureText("PLAY GAME", 30)/2, screenHeight/2, 30, playColor);
                    DrawText("QUIT", screenWidth/2 - MeasureText("QUIT", 30)/2, screenHeight/2 + 50, 30, quitColor);

                    if (titleMenuOption == 0) DrawText(">", screenWidth/2 - MeasureText("PLAY GAME", 30)/2 - 30, screenHeight/2, 30, YELLOW);
                    if (titleMenuOption == 1) DrawText(">", screenWidth/2 - MeasureText("QUIT", 30)/2 - 30, screenHeight/2 + 50, 30, YELLOW);

                    // draw some text using the default font
                    //DrawText("Hello Raylib", 200,200,20,WHITE);

                    // draw our texture to the screen
                    //DrawTexture(wabbit, 400, 200, WHITE);

                } break;
                case GAMEPLAY:
                {
                    DrawRectangle(0, 0, screenWidth, screenHeight, PURPLE);
                    DrawText(TextFormat("GAMEPLAY - LEVEL %d", currentLevel), 20, 20, 40, MAROON);
                    DrawText("PRESS ENTER to WIN LEVEL / ESC to PAUSE", screenWidth/2 - MeasureText("PRESS ENTER to WIN LEVEL / ESC to PAUSE", 20)/2, screenHeight - 100, 20, MAROON);

                } break;
                case PAUSE:
                {
                    // Draw gameplay background dimmed (optional logic, hard to do with switch unless we draw gameplay first)
                    // For simplicity, just a grey background
                    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));
                    
                    DrawText("PAUSED", screenWidth/2 - MeasureText("PAUSED", 40)/2, screenHeight/4, 40, WHITE);

                    // Draw Menu
                    Color resumeColor = (pauseMenuOption == 0) ? YELLOW : WHITE;
                    Color quitColor = (pauseMenuOption == 1) ? YELLOW : WHITE;

                    DrawText("RESUME", screenWidth/2 - MeasureText("RESUME", 30)/2, screenHeight/2, 30, resumeColor);
                    DrawText("QUIT TO TITLE", screenWidth/2 - MeasureText("QUIT TO TITLE", 30)/2, screenHeight/2 + 50, 30, quitColor);

                    if (pauseMenuOption == 0) DrawText(">", screenWidth/2 - MeasureText("RESUME", 30)/2 - 30, screenHeight/2, 30, YELLOW);
                    if (pauseMenuOption == 1) DrawText(">", screenWidth/2 - MeasureText("QUIT TO TITLE", 30)/2 - 30, screenHeight/2 + 50, 30, YELLOW);

                } break;
                case ENDING:
                {
                    // TODO: Draw ENDING screen here!
                    DrawRectangle(0, 0, screenWidth, screenHeight, BLUE);
                    DrawText("ENDING SCREEN", screenWidth/2 - MeasureText("ENDING SCREEN", 40)/2, screenHeight/4, 40, LIGHTGRAY);
                    DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", screenWidth/2 - MeasureText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 20)/2, screenHeight - 100, 20, LIGHTGRAY);

                } break;
                case CREDIT:
                {
                    DrawRectangle(0, 0, screenWidth, screenHeight, DARKGRAY);
                    DrawText("CREDITS", screenWidth/2 - MeasureText("CREDITS", 40)/2, screenHeight/4, 40, LIGHTGRAY);
                    DrawText("This game was developed by Reino de Aragon", screenWidth/2 - MeasureText("This game was developed by Reino de Aragon", 20)/2, screenHeight/2, 20, LIGHTGRAY);
                    DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", screenWidth/2 - MeasureText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 20)/2, screenHeight - 100, 20, LIGHTGRAY);

                } break;
                default: break;
            }
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(wabbit);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
