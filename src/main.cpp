#include "enemy.h"
#include "particles.h"
#include "platform.h"
#include "player.h"
#include "raylib.h"
#include "resource_dir.h"
#include "star_donut.h"
#include "sunlight.h"
#include <iostream>
#include <string> // Added for to_string
#include <tuple>
#include <vector>

// --- CONSTANTS & ENUMS ---
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 800;

extern bool playerToggle;

typedef enum GameScreen {
  LOGO = 0,
  TITLE,
  STORY,
  GAMEPLAY,
  PAUSE,
  GAME_OVER,
  ENDING,
  CREDIT
} GameScreen;

typedef struct Cloud {
  Rectangle rect;
  float speed;
  float leftLimit;
  float rightLimit;
  bool movingRight;
} Cloud;

struct Level {
  Vector2 spawnPoint;
	std::vector<Enemy> enemies;
  std::vector<Platform> platforms;
  std::vector<Cloud> clouds;
  Vector2 sunPosition;
  std::tuple<Texture2D, Texture2D> backgrounds;
  std::pair<std::string, std::string> musicPath;
	bool isDay;
  Rectangle exitZone;
};

// --- GLOBAL STATE (Wrapped in a struct for cleanliness) ---
struct GameState {
    GameScreen currentScreen = GAMEPLAY;
    int currentLevelIndex = 0;
    bool exitGame = false;
    int menuOption = 0; // Shared for Title and Pause
    int framesCounter = 0;
    int burnTimer = 0;
    
    // Player State
    Player player = {0};

    Texture2D texSpider;
    Texture2D texRoach;

    // Platform Textures
    Texture2D texBasic;
    Texture2D texMushroom;
    Texture2D texFlower;
    Texture2D texGameOver;
    Texture2D texGameOver2;
    
    // Level Data
    std::vector<Level> levels;
    
    // Visuals
    StarDonutState donutState;
    CircularBuffer particleSystem;
    RenderTexture2D lightLayer;
    bool isPlayerBurning = false;

	std::pair<Music, Music> music;
	bool musicToggle;
};

// --- HELPER FUNCTIONS FOR INITIALIZATION ---
//
std::vector<Level> InitLevels() {
  std::vector<Level> levels;

    // LEVEL 1
    Level lvl1;

    Image dayImg = LoadImage("level1_daybackground.png");
    Image nightImg = LoadImage("level1_nightbackground.jpg");

    ImageResize(&dayImg, SCREEN_WIDTH, SCREEN_HEIGHT);
    ImageResize(&nightImg, SCREEN_WIDTH, SCREEN_HEIGHT);

    Texture2D dayTex = LoadTextureFromImage(dayImg);
    Texture2D nightTex = LoadTextureFromImage(nightImg);

    UnloadImage(dayImg);
    UnloadImage(nightImg);

    lvl1.backgrounds = std::make_tuple(dayTex, nightTex);
    lvl1.spawnPoint = {SCREEN_WIDTH - 50, (float)(SCREEN_HEIGHT - 100)};
    lvl1.platforms = {
        {{SCREEN_WIDTH - 550, SCREEN_HEIGHT/2.0f, 550, 30}, basic},
        {{0, SCREEN_HEIGHT/2.0f, 300, 30}, basic},
        {{400, SCREEN_HEIGHT - 125, 100, 100}, mushroom},
        {{0, (float)(SCREEN_HEIGHT - 70), (float)SCREEN_WIDTH, 50}, invisible}};
    lvl1.sunPosition = {SCREEN_WIDTH - 100, -50.0f};
	  lvl1.isDay = true;
    lvl1.exitZone = {50, (float)(SCREEN_HEIGHT/2.0f) - 65, 100, 50};
	lvl1.musicPath = {"music/lvlupjam_lvl1.wav", "music/lvlupjam_lvl1_night.wav"};
    levels.push_back(lvl1);

    // LEVEL 2
    Level lvl2;

    Image dayImg2 = LoadImage("level2_daybackground.png");
    Image nightImg2 = LoadImage("level2_nightbackground.png");

    ImageResize(&dayImg2, SCREEN_WIDTH, SCREEN_HEIGHT);
    ImageResize(&nightImg2, SCREEN_WIDTH, SCREEN_HEIGHT);

    Texture2D dayTex2 = LoadTextureFromImage(dayImg2);
    Texture2D nightTex2 = LoadTextureFromImage(nightImg2);

    UnloadImage(dayImg2);
    UnloadImage(nightImg2);

    lvl2.backgrounds = std::make_tuple(dayTex2, nightTex2);

    lvl2.spawnPoint = {50, (float)(SCREEN_HEIGHT - 100)};
    lvl2.platforms = {
        {{SCREEN_WIDTH-250, (SCREEN_HEIGHT-FLOWER_HEIGHT), FLOWER_WIDTH, FLOWER_HEIGHT}, flower},
        {{0, 250, 200, 30}, basic},
        {{300, 350, 200, 30}, basic},
        {{600, 450, 200, 30}, basic},
        {{600, (float)(SCREEN_HEIGHT - 50), 100, 100}, basic},// special fake platform for mobe
        {{0, (float)(SCREEN_HEIGHT - 50), (float)SCREEN_WIDTH, 50}, basic},
	};
	int fake_platform_idx = 4;

    lvl2.enemies = {{{lvl2.platforms[fake_platform_idx].position.x, lvl2.platforms[fake_platform_idx].position.y}, {20, 100}, true, fake_platform_idx}};
    lvl2.sunPosition = {800.0f, -50.0f};
    lvl2.exitZone = {25, 200, 100, 50};
	lvl2.isDay = false;
	lvl2.musicPath = {"music/lvlupjam_lvl2.wav", "music/lvlupjam_lvl2_night.wav"};
    levels.push_back(lvl2);

    // LEVEL 3
    Level lvl3;

    Image dayImg3 = LoadImage("level3_daybackground.png");
    Image nightImg3 = LoadImage("level3_nightbackground.png");

    ImageResize(&dayImg3, SCREEN_WIDTH, SCREEN_HEIGHT);
    ImageResize(&nightImg3, SCREEN_WIDTH, SCREEN_HEIGHT);

    Texture2D dayTex3 = LoadTextureFromImage(dayImg3);
    Texture2D nightTex3 = LoadTextureFromImage(nightImg3);

    UnloadImage(dayImg3);
    UnloadImage(nightImg3);

    lvl3.backgrounds = std::make_tuple(dayTex3, nightTex3);

    lvl3.spawnPoint = {100, 600};
    lvl3.platforms = {
        {{100, 650, 150, 10}, basic},
        {{400, 600, 100, 10}, basic},
        {{600, 500, 100, 10}, basic},
        {{800, 400, 100, 10}, basic},
        {{1000, 300, 100, 10}, basic},
        {{0, (float)(SCREEN_HEIGHT + 200), (float)SCREEN_WIDTH, 50}, basic}};
    lvl3.enemies = {{{lvl3.platforms[2].position.x + lvl3.platforms[2].position.width / 2, lvl3.platforms[2].position.y}, {30, 30}, false, 2}};
    lvl3.clouds = {{{300, 200, 150, 40}, 200.0f, 200, 900, true}, {{600, 400, 150, 40}, 200.0f, 300, 1000, false}};
    lvl3.sunPosition = {200.0f, -50.0f};
	lvl3.isDay = true;
	lvl3.musicPath = {"music/lvlupjam_lvl1.wav", "music/lvlupjam_lvl1_night.wav"};
    levels.push_back(lvl3);

  return levels;
}

void ResetPlayer(GameState &game) {
  if (game.currentLevelIndex < (int)game.levels.size()) {
    game.player.position = game.levels[game.currentLevelIndex].spawnPoint;
    game.player.speed = 0;
    game.player.healthPoints = 1000;
    game.player.toggleCooldown = 2.0f;
  }
}

void selectPlayMusic(GameState& game) {
	if (game.musicToggle != playerToggle) {
		game.musicToggle = playerToggle;
		if (!playerToggle) {
			StopMusicStream(game.music.first);
			PlayMusicStream(game.music.second);
		} else {
			StopMusicStream(game.music.second);
			PlayMusicStream(game.music.first);
		}
	}
}

// --- LOGIC UPDATES ---
void changeMusicStreams(GameState &game) {
	StopMusicStream(game.music.first);
	StopMusicStream(game.music.second);

	Level& level = game.levels[game.currentLevelIndex];
	game.music.first = LoadMusicStream(level.musicPath.first.c_str());
	game.music.second = LoadMusicStream(level.musicPath.second.c_str());
	game.musicToggle = !playerToggle;
}

void UpdateMenu(GameState &game, bool isPauseMenu) {
  if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
    game.menuOption++;
  if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
    game.menuOption--;

  if (game.menuOption < 0)
    game.menuOption = 1;
  if (game.menuOption > 1)
    game.menuOption = 0;

  if (IsKeyPressed(KEY_ENTER)) {
    if (isPauseMenu) {
      if (game.menuOption == 0)
        game.currentScreen = GAMEPLAY; // RESUME
      else if (game.menuOption == 1) { // QUIT TO TITLE
        game.currentScreen = TITLE;
        game.menuOption = 0;
      }
    } else { // Title Menu
      if (game.menuOption == 0)
        game.currentScreen = STORY; // PLAY
      else if (game.menuOption == 1)
        game.exitGame = true; // QUIT
    }
  }
}

void UpdateGameplay(GameState &game, float delta) {
  if (IsKeyPressed(KEY_ESCAPE)) {
    game.currentScreen = PAUSE;
    game.menuOption = 0;
    return;
  }

  // Prepare Level Data
  Level &currentLvlData = game.levels[game.currentLevelIndex];

  // Check Burning logic (1 HP per 60 frames)
    if (playerToggle && game.isPlayerBurning) {
        game.burnTimer++;
        // EMIT PARTICLE (Every 15 frames)
        if (game.burnTimer % 15 == 0) {
            // Emitting from roughly the center of the player
            Vector2 firePos = { game.player.position.x, game.player.position.y - game.player.size.y / 2 };
            EmitParticle(&game.particleSystem, firePos, FIRE);
        }
        if (game.burnTimer >= 60) {
            game.player.healthPoints -= 1;
            game.burnTimer = 0;
            //std::cout << "Player burned! HP: " << game.player.healthPoints << std::endl;
        }
    } else {
        //game.burnTimer = 0; // Optional: Reset timer if they find shade
    }
    // Check for specific death condition if needed, or rely on player update
    if (game.player.healthPoints <= 0) {
        // Handle death (e.g., reset level or game over)
        game.currentScreen = GAME_OVER;
    }

  // 1. Update Player
  updatePlayer(game.player, currentLvlData.platforms,
    currentLvlData.enemies, delta, currentLvlData.spawnPoint);

  // 3. Update Clouds & Player Cloud Riding
  for (auto& cloud : currentLvlData.clouds) {
      // Move Cloud
      float moveAmount = cloud.speed * delta;
      if (cloud.movingRight) {
          cloud.rect.x += moveAmount;
          if (cloud.rect.x > cloud.rightLimit) cloud.movingRight = false;
      } else {
          cloud.rect.x -= moveAmount;
          if (cloud.rect.x < cloud.leftLimit) cloud.movingRight = true;
      }

    // Riding logic
    if (game.player.speed >= 0) { // Only if falling or standing
      if (CheckCollisionRecs(
              {game.player.position.x, game.player.position.y, 1, 1},
              cloud.rect)) {
        // Simple overlap check often fails for "feet", prefer checking feet
        // explicitly:
        float feetY = game.player.position.y;
        if (feetY >= cloud.rect.y && feetY <= cloud.rect.y + 15.0f &&
            game.player.position.x >= cloud.rect.x &&
            game.player.position.x <= cloud.rect.x + cloud.rect.width) {

          game.player.speed = 0.0f;
          game.player.position.y = cloud.rect.y;
          game.player.canJump = true;
          game.player.position.x +=
              (cloud.movingRight ? moveAmount : -moveAmount);
        }
      }
    }
  }

    // 4. Level Transition Test
	// TODO: add a winning condition instead of Key press
	int currentLevelIdx = game.currentLevelIndex;
    if (IsKeyPressed(KEY_ENTER)) {
        game.currentLevelIndex++;

        if (game.currentLevelIndex >= (int)game.levels.size()) {
            game.currentScreen = ENDING;
            game.currentLevelIndex = 0; // Reset for next time
        } else {
            ResetPlayer(game);
        }
    }

    Rectangle playerRect = {game.player.position.x - game.player.size.x/2, game.player.position.y - game.player.size.y, game.player.size.x, game.player.size.y};
    // Check collision against the level's exit zone
    if (CheckCollisionRecs(playerRect, currentLvlData.exitZone)) {
        game.currentLevelIndex++;
        if (game.currentLevelIndex >= (int)game.levels.size()) {
            game.currentScreen = ENDING;
            game.currentLevelIndex = 0; 
        } else {
            ResetPlayer(game);
        }
    }

	if (currentLevelIdx != game.currentLevelIndex) {
		playerToggle = game.levels[game.currentLevelIndex].isDay;
		changeMusicStreams(game);
	}
}

// --- DRAWING FUNCTIONS ---
void DrawLighting(GameState &game, Level &level) {
  // If it is Night, do not draw light rays and do not burn player
  if (!playerToggle) {
      game.isPlayerBurning = false;
      BeginTextureMode(game.lightLayer);
      ClearBackground(BLANK);
      EndTextureMode();
      return;
  }
  
  // 3D Shadow Casting Logic
  Vector2 sunPos = level.sunPosition;
  BoundingBox playerBox3D = {
      (Vector3){game.player.position.x - game.player.size.x / 2,
                game.player.position.y - game.player.size.y, -10},
      (Vector3){game.player.position.x + game.player.size.x / 2,
                game.player.position.y, 10}};

  BeginTextureMode(game.lightLayer);
  ClearBackground(BLANK);

  bool anyRayHitPlayer = false;

  // Raycast loop
  for (int x = -200; x <= SCREEN_WIDTH + 200; x += 10) {
    Vector2 targetPos = {(float)x, (float)SCREEN_HEIGHT};
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

    // Check Player
    RayCollision playerHit = GetRayCollisionBox(currentRay, playerBox3D);
    if (playerHit.hit && playerHit.distance < nearestDist) {
      nearestDist = playerHit.distance;
      hitPlayer = true;
    }

    // Check Platforms
    for (const auto &plat : level.platforms) {
      BoundingBox platBox = {(Vector3){plat.position.x, plat.position.y, -10},
                             (Vector3){plat.position.x + plat.position.width,
                                       plat.position.y + plat.position.height,
                                       10}};
      RayCollision platHit = GetRayCollisionBox(currentRay, platBox);
      if (platHit.hit && platHit.distance < nearestDist) {
        nearestDist = platHit.distance;
        hitPlayer = false;
      }
    }

    // Check Clouds
    for (const auto &cloud : level.clouds) {
      BoundingBox cloudBox = {(Vector3){cloud.rect.x, cloud.rect.y, -10},
                              (Vector3){cloud.rect.x + cloud.rect.width,
                                        cloud.rect.y + cloud.rect.height, 10}};
      RayCollision cloudHit = GetRayCollisionBox(currentRay, cloudBox);
      if (cloudHit.hit && cloudHit.distance < nearestDist) {
        nearestDist = cloudHit.distance;
        hitPlayer = false;
      }
    }

    Vector2 endPos = {sunPos.x + rayDir.x * (nearestDist + 20.0f),
                      sunPos.y + rayDir.y * (nearestDist + 20.0f)};
    if (hitPlayer) {
      rayColor = RED;
      anyRayHitPlayer = true;
      endPos = {sunPos.x + rayDir.x * nearestDist,
                sunPos.y + rayDir.y * nearestDist};
    }
    DrawLineEx(sunPos, endPos, 12.0f, rayColor);
    if (hitPlayer)
      DrawCircleV(endPos, 5, RED);
  }
  EndTextureMode();

  game.isPlayerBurning = anyRayHitPlayer;

  // Draw the texture to screen
  DrawTextureRec(game.lightLayer.texture,
                 (Rectangle){0, 0, (float)SCREEN_WIDTH, (float)-SCREEN_HEIGHT},
                 (Vector2){0, 0}, Fade(WHITE, 0.2f));
}

void DrawGameplay(GameState &game) {
  Level &currentLvlData = game.levels[game.currentLevelIndex];

  // Background
  Texture2D bg = playerToggle ? std::get<0>(currentLvlData.backgrounds) : std::get<1>(currentLvlData.backgrounds);
	// TODO: possible to fade in?
  if (bg.id != 0) {
		DrawTexture(bg, 0, 0, WHITE);
	} else {
		DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, PURPLE);
	}

  // Platforms
  for (const auto& plat : currentLvlData.platforms) {
		// Pick the texture based on type
      // Texture2D *t = &game.texBasic; // Default
      // if (plat.type == mushroom) t = &game.texMushroom;
      // if (plat.type == flower) t = &game.texFlower;

      // Calculate source (full texture)
      // Rectangle source = {0, 0, (float)t->width, (float)t->height};
      
      // Calculate destination (platform position)
      // Rectangle dest = plat.position;

      // Draw Texture
      //DrawTexturePro(*t, source, dest, {0, 0}, 0.0f, WHITE);
      
      // Optional: Keep hitbox for debug
      // DrawRectangleLinesEx(dest, 2.0f, RED);
      // Pass the textures here. 
      // If you don't have one loaded yet, just pass "0" or a blank Texture2D
      drawPlatform(plat, game.texMushroom, game.texFlower, game.texBasic);
  }


  // Draw Exit Zone
  DrawRectangleRec(currentLvlData.exitZone, Fade(GREEN, 0.3f)); // Transparent green fill
  DrawRectangleLinesEx(currentLvlData.exitZone, 2.0f, GREEN);   // Solid outline
  DrawText("EXIT", (int)currentLvlData.exitZone.x + 5, (int)currentLvlData.exitZone.y - 20, 20, GREEN);

  // Clouds
  for (const auto &cloud : currentLvlData.clouds) {
    DrawRectangleRec(cloud.rect, Fade(SKYBLUE, 0.9f));
    DrawRectangleLinesEx(cloud.rect, 2, WHITE);
  }

	drawPlayer(game.player);

// Player (Primitive drawing, can be replaced with sprite logic)
  Vector2 playerPosVisual = {game.player.position.x - game.player.size.x / 2, game.player.position.y - game.player.size.y};
  // DrawRectangleV(playerPosVisual, game.player.size, BLUE);
  // DrawRectangleLinesEx((Rectangle){playerPosVisual.x, playerPosVisual.y, game.player.size.x, game.player.size.y}, 2.0f, BLACK);
  // --- HP BAR ---
  float barWidth = 6.0f;
  float barHeight = 50.0f; 
  float barX = playerPosVisual.x + game.player.size.x + 6.0f; // Position to the right of player
  float barY = playerPosVisual.y + (game.player.size.y - barHeight) / 2.0f; // Center vertically relative to player

  // Draw Background
  DrawRectangle(barX, barY, barWidth, barHeight, Fade(BLACK, 0.5f));
  DrawRectangleLines(barX, barY, barWidth, barHeight, BLACK);

  // Draw Health Fill
  float maxHP = 5.0f;
  float hpPct = (float)game.player.healthPoints / maxHP;
  if (hpPct < 0.0f) hpPct = 0.0f;

  float fillHeight = barHeight * hpPct;
  float fillY = barY + (barHeight - fillHeight); // Fill from bottom up

  Color barColor = GREEN;
  if (game.player.healthPoints <= 2) barColor = RED;
  else if (game.player.healthPoints <= 4) barColor = ORANGE;

  DrawRectangle(barX, fillY, barWidth, fillHeight, barColor);
  // --------------

  // Light System
  DrawLighting(game, currentLvlData);

  // Sun & Enemy
  DrawCircleV(currentLvlData.sunPosition, 40, YELLOW);
	if (isEnemyActive()) {
		for (Enemy &enemy : currentLvlData.enemies) {
			int pi = enemy.patrolPlatformIndex;
			  // 1. Select the correct texture
        Texture2D currentTex = (enemy.type == spider) ? game.texSpider : game.texRoach;
          
        // 2. Call the function (Updates position AND Draw)
        updateEnemy(enemy, currentLvlData.platforms[pi].position, GetFrameTime(), currentTex);
		}
	}

	// Particles
	UpdateParticles(&game.particleSystem);
	// if (game.isPlayerBurning) {
	// 	Vector2 center = {game.player.position.x, game.player.position.y - game.player.size.y / 2};
	// 	EmitParticle(&game.particleSystem, center, FIRE);
	// 	EmitParticle(&game.particleSystem, center, FIRE);
	// 	DrawText("BURNING!", SCREEN_WIDTH / 2, 50, 40, RED);
	// } else {
	// 	DrawText("SAFE", SCREEN_WIDTH / 2, 50, 40, GREEN);
	// }
	DrawParticles(&game.particleSystem);

	// Debug
	DrawText(TextFormat("Level: %d", game.currentLevelIndex + 1), 10, 10, 20, WHITE);
	bool isDay = currentLvlData.isDay;
	DrawText(TextFormat("isDay: %d", isDay), 60, 60, 40, WHITE);
}

void DrawMenu(const char* title, const char* opt1, const char* opt2, int selection) {
	DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKGREEN);
	DrawText(title, SCREEN_WIDTH / 2 - MeasureText(title, 30) / 2, SCREEN_HEIGHT / 3, 30, YELLOW);

	Color color1 = (selection == 0) ? YELLOW : WHITE;
	Color color2 = (selection == 1) ? YELLOW : WHITE;

	// Draw Options
	DrawText(opt1, SCREEN_WIDTH / 2 - MeasureText(opt1, 30) / 2, SCREEN_HEIGHT / 2, 30, color1);
	DrawText(opt2, SCREEN_WIDTH / 2 - MeasureText(opt2, 30) / 2, SCREEN_HEIGHT / 2 + 50, 30, color2);

	// Draw Cursor
	int yPos = (selection == 0) ? SCREEN_HEIGHT / 2 : SCREEN_HEIGHT / 2 + 50;
	const char* selectedText = (selection == 0) ? opt1 : opt2;
	DrawText(">", SCREEN_WIDTH / 2 - MeasureText(selectedText, 30) / 2 - 30, yPos, 30, YELLOW);
}

// --- MAIN ---
int main() {
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LVLUP Game Jam 2026");
	SetExitKey(KEY_NULL);
	SearchAndSetResourceDir("resources");

  playerToggle = true;

	GameState game;
  game.player.texture = LoadTexture("my_player.png");
  game.texGameOver = LoadTexture("game_over.png"); 
  // LOAD PLATFORM TEXTURES (Make sure you have these files!)
  //game.texBasic = LoadTexture("platform_basic.png");
  game.texMushroom = LoadTexture("platform_mushroom.png");
  game.texFlower = LoadTexture("platform_flower.png");

  game.texSpider = LoadTexture("spider.png"); 
  game.texRoach = LoadTexture("roach.png");   
  game.texGameOver2 = LoadTexture("gameOverScreen.png"); 


	game.levels = InitLevels();
	game.player.size = {100, 160};
	ResetPlayer(game);

	InitParticles();
	game.particleSystem = InitParticles();
	InitStarDonut(&game.donutState, SCREEN_WIDTH, SCREEN_HEIGHT);
	game.lightLayer = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

	InitAudioDevice();
	game.music.first = LoadMusicStream(game.levels[0].musicPath.first.c_str());
	game.music.second = LoadMusicStream(game.levels[0].musicPath.second.c_str());
	
	playerToggle = true;
	game.musicToggle = !playerToggle;
	selectPlayMusic(game);

	while (!WindowShouldClose() && !game.exitGame) {
		float delta = GetFrameTime();

		// --- UPDATE ---
		UpdateMusicStream(game.music.first);
		UpdateMusicStream(game.music.second);

		switch (game.currentScreen) {
			case LOGO:
				game.framesCounter++;
				if (game.framesCounter > 60) game.currentScreen = TITLE;
				break;
			case TITLE:
				UpdateMenu(game, false);
				break;
			case STORY:
				if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
					ResetPlayer(game); // Ensure freshness
					game.currentScreen = GAMEPLAY;
				}
				break;
			case GAMEPLAY:
				UpdateGameplay(game, delta);
				break;
			case PAUSE:
				UpdateMenu(game, true);
				if (IsKeyPressed(KEY_ESCAPE)) game.currentScreen = GAMEPLAY;
				break;
      case GAME_OVER:
        if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER)) {
            ResetPlayer(game);
            game.currentScreen = GAMEPLAY;
        }
        break;
			case ENDING:
				if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) game.currentScreen = CREDIT;
				break;
			case CREDIT:
				if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) game.currentScreen = TITLE;
				break;
			default: break;
		}

		selectPlayMusic(game);
				// --- DRAW ---
		BeginDrawing();
		ClearBackground(BLACK);

		switch (game.currentScreen) {
			case LOGO:
				DrawStarDonut(&game.donutState, SCREEN_WIDTH, SCREEN_HEIGHT);
				break;
			case TITLE:
				DrawMenu("TITLE SCREEN", "PLAY GAME", "QUIT", game.menuOption);
				break;
			case STORY:
				DrawText("STORY SCREEN", 20, 20, 40, LIGHTGRAY);
				break;
			case GAMEPLAY:
				DrawGameplay(game);
				break;
			case PAUSE:
				// Draw gameplay behind pause menu (simulated by re-drawing specialized simple bg or just overlay)
				// Ideally, capture gameplay to a texture, but for now simple overlay:
				DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8f));
				DrawMenu("PAUSED", "RESUME", "QUIT TO TITLE", game.menuOption);
				break;
      case GAME_OVER:
      {
        // Draw the image to fit the screen
        DrawTexturePro(game.texGameOver, 
            (Rectangle){0,0, (float)game.texGameOver.width, (float)game.texGameOver.height}, 
            (Rectangle){0,0, SCREEN_WIDTH, SCREEN_HEIGHT}, 
            (Vector2){0,0}, 0.0f, WHITE);
        
        DrawTexturePro(game.texGameOver2, 
            (Rectangle){0,0, (float)game.texGameOver2.width, (float)game.texGameOver2.height}, 
            (Rectangle){SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/4 - 100, 400, 200}, 
            (Vector2){0,0}, 90.0f, WHITE);
        
        // Instructions
        const char* text = "PRESS 'R' TO RESTART";
        DrawText(text, SCREEN_WIDTH/2 - MeasureText(text, 30)/2, SCREEN_HEIGHT - 100, 30, WHITE);
      }
        break;
			case ENDING:
				DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLUE);
				DrawText("ENDING SCREEN", SCREEN_WIDTH/2 - MeasureText("ENDING SCREEN", 40)/2, SCREEN_HEIGHT/4, 40, LIGHTGRAY);
				DrawText("PRESS ENTER", SCREEN_WIDTH/2 - MeasureText("PRESS ENTER", 20)/2, SCREEN_HEIGHT - 100, 20, LIGHTGRAY);
				break;
			case CREDIT:
				DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKGRAY);
				DrawText("CREDITS", SCREEN_WIDTH/2 - MeasureText("CREDITS", 40)/2, SCREEN_HEIGHT/4, 40, LIGHTGRAY);
				DrawText("Developed by Reino de Aragon", SCREEN_WIDTH/2 - MeasureText("Developed by Reino de Aragon", 20)/2, SCREEN_HEIGHT/2, 20, LIGHTGRAY);
				break;
			default: break;
		}

		EndDrawing();
	}
  for (auto &level : game.levels) {
    UnloadTexture(std::get<0>(level.backgrounds));
    UnloadTexture(std::get<1>(level.backgrounds));
    }
  UnloadRenderTexture(game.lightLayer);
  UnloadTexture(game.texMushroom);
  UnloadTexture(game.texFlower);
UnloadRenderTexture(game.lightLayer);
  UnloadTexture(game.player.texture);
  UnloadTexture(game.texSpider);
  UnloadTexture(game.texRoach);

	UnloadMusicStream(game.music.first);
	UnloadMusicStream(game.music.second);
	CloseWindow();
	return 0;
}
