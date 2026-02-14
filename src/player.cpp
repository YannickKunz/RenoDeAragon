#pragma once
#include "raylib.h"
#include "platform.cpp"
#include "enemy.cpp"

#define TOGGLE_DELAY_SEC 2.0f
#define G 800
#define JUMP_SPEED 600.0f
#define MOVEMENT 200.0f
#define MUSHROOM_COEFF 1.2f

typedef struct Player {
	Vector2 position;
	Vector2 size;
	float speed;
	bool canJump;
	bool toggle;
	float toggleCooldown;
	int healthPoints;
} Player;

void updatePlayer(Player &player, Platform *platforms,
		int platformsLength, Enemy &enemy, const float delta, 
	Vector2 spawnPoint) {
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

	// Check horizontal collisions (player sides vs platform sides)
	for (int i = 0; i < platformsLength; i++) {
		Rectangle platformPosition = platforms[i].position;
		float playerLeft = player.position.x - player.size.x / 2;
		float playerRight = player.position.x + player.size.x / 2;
		float playerTop = player.position.y - player.size.y;
		float playerBottom = player.position.y;

		// Check if vertically overlapping
		if (playerBottom > platformPosition.y && playerTop < platformPosition.y + platformPosition.height) {
			// Check if horizontally overlapping
			if (playerRight > platformPosition.x && playerLeft < platformPosition.x + platformPosition.width) {
				// Push out from whichever side is closer
				float overlapLeft = playerRight - platformPosition.x;
				float overlapRight = (platformPosition.x + platformPosition.width) - playerLeft;
				if (overlapLeft < overlapRight) {
					player.position.x = platformPosition.x - player.size.x / 2; // push left
				} else {
					player.position.x =
						platformPosition.x + platformPosition.width + player.size.x / 2; // push right
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
	for (int i = 0; i < platformsLength; i++) {
		Rectangle platformPosition = platforms[i].position;
		float playerLeft = player.position.x - player.size.x / 2;
		float playerRight = player.position.x + player.size.x / 2;
		if (playerRight >= platformPosition.x && playerLeft <= (platformPosition.x + platformPosition.width)) {
			// Landing on top of platform (falling down)
			if (player.speed > 0 && platformPosition.y >= player.position.y &&
					platformPosition.y <= (player.position.y + player.speed * delta)) {
				if (platforms[i].type == mushroom) {
					// jump
					player.speed = - MUSHROOM_COEFF * JUMP_SPEED;
				} else {
					player.speed = 0.0f;
					player.position.y = platformPosition.y;
					hitObstacle = true;
				}
				break;
			}
			// Head hitting bottom of platform (jumping up)
			float playerHead = player.position.y - player.size.y;
			float newHead = playerHead + player.speed * delta;
			float platBottom = platformPosition.y + platformPosition.height;
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
