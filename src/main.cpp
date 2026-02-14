#include "raylib.h"
#include <iostream>
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#define TOGGLE_DELAY_SEC 2.0f

typedef struct Player {
	Vector2 position;
	Vector2 size;
	bool toggle;
	float toggleCooldown;
} Player;

void updatePlayer(Player& player) {
	player.toggleCooldown += GetFrameTime(); // could this overflow?
	if (IsKeyDown(KEY_A)) player.position.x -= 5.0f;
	if (IsKeyDown(KEY_D)) player.position.x += 5.0f;
	if (IsKeyPressed(KEY_F) && (player.toggleCooldown >= TOGGLE_DELAY_SEC)) {
		player.toggle = !player.toggle;
		player.toggleCooldown = 0.0f;
	}

	DrawRectangleV(player.position, player.size, BLUE);
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
	
	Player player = { 0 };
	player.position = { 50, screenHeight - 100 };
	player.size = { 50, 50 };
	player.toggleCooldown = 2.0f;

	Rectangle platforms[] = {
		{200, 300, 100, 10},
		{300, 400, 100, 10},
		{0, screenHeight - 50, screenWidth, 50},
	};

	int platformsLength = sizeof(platforms)/sizeof(platforms[0]);

	// game loop
	while (!WindowShouldClose()) {		// run the loop until the user presses ESCAPE or presses the Close button on the window
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// draw some text using the default font
		std::string debugText = "Debug coordinates: " + std::to_string(player.position.x) 
			+ ", " + std::to_string(player.position.y)
			+ "\nToggle: " + std::to_string(player.toggle)
			+ ", Time: " + std::to_string(player.toggleCooldown);

		DrawText(debugText.c_str(), 10, 10, 20, WHITE);

		updatePlayer(player);

		for (int i = 0; i < platformsLength; i++) {
			Rectangle rec = platforms[i];
			DrawRectangleRec(rec, GRAY);
		}
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}
	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
