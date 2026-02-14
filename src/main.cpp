#include "raylib.h"
#include <iostream>
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#define TOGGLE_DELAY_SEC 2.0f
#define G 800
#define JUMP_SPEED 600.0f
#define MOVEMENT 200.0f

typedef struct Player {
	Vector2 position;
	Vector2 size;
	float speed;
	bool canJump;
	bool toggle;
	float toggleCooldown;
} Player;

void updatePlayer(Player& player, Rectangle (&platforms)[], int platformsLength) {
	float delta = GetFrameTime();
	player.toggleCooldown += delta; // could this overflow?
	if (IsKeyDown(KEY_A)) player.position.x -= MOVEMENT*delta;
	if (IsKeyDown(KEY_D)) player.position.x += MOVEMENT*delta;
	if (IsKeyPressed(KEY_F) && (player.toggleCooldown >= TOGGLE_DELAY_SEC)) {
		player.toggle = !player.toggle;
		player.toggleCooldown = 0.0f;
	}
	if (IsKeyDown(KEY_SPACE) && player.canJump) {
		player.speed -= JUMP_SPEED;
		player.canJump = false;
	}

	bool hitObstacle = false;
	for (int i = 0; i < platformsLength; i++) {
		Rectangle plat = platforms[i];
		if (plat.x <= player.position.x && (plat.x + plat.width) >= player.position.x) {
			if (plat.y >= player.position.y && plat.y <= (player.position.y + player.speed * delta)) {
				hitObstacle = true;
				player.speed = 0.0f;
				player.position.y = plat.y;
				break;
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

	Vector2 playerPosition = { player.position.x - player.size.x/2, player.position.y - player.size.y };
	DrawRectangleV(playerPosition, player.size, BLUE);
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
		{400, 600, 100, 10},
		{600, 500, 100, 10},
		{0, screenHeight - 50, screenWidth, 50},
	};

	int platformsLength = sizeof(platforms)/sizeof(platforms[0]);

	// game loop
	SetTargetFPS(60);
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

		updatePlayer(player, platforms, platformsLength);
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
