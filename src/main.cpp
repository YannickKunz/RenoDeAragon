#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

typedef struct Player {
	Vector2 position;
	Vector2 size;
} Player;

void checkKeyboardInput(Player& player) {
	if (IsKeyDown(KEY_A)) player.position.x -= 5.0f;
	if (IsKeyDown(KEY_D)) player.position.x += 5.0f;
}

int main () {
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	const int screenWidth = 1280;
	const int screenHeight = 800;
	InitWindow(screenWidth, screenHeight, "LVLUP Game Jam 2026");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");
	
	Player player = { 0 };
	player.position = {x: 50, y: screenHeight - 100 };
	player.size = { 80, 80 };
	// game loop
	while (!WindowShouldClose()) {		// run the loop until the user presses ESCAPE or presses the Close button on the window
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// draw some text using the default font
		DrawText("Demo Text", 10,10,20,WHITE);

		checkKeyboardInput(player);
		DrawRectangleV(player.position, player.size, BLUE);
		// draw our texture to the screen
		// DrawTextureV(wabbit, position, WHITE);
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}
	// unload our texture so it can be cleaned up
	UnloadTexture(wabbit);
	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
