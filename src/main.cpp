#include "enemy.cpp"
#include "particles.cpp"
#include "platform.cpp"
#include "player.cpp"
#include "raylib.h"
#include "resource_dir.h"
#include "star_donut.h" 
#include "sunlight.cpp"
#include <iostream>
#include <tuple>
#include <vector>
#include <string> 

// --- CONSTANTS & ENUMS ---
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 800;

typedef enum GameScreen {
    LOGO = 0,
    TITLE,
    STORY,
    GAMEPLAY,
    PAUSE,
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
    Enemy enemy;
    std::vector<Platform> platforms;
    std::vector<Cloud> clouds;
    Vector2 sunPosition;
    std::tuple<Texture2D, Texture2D> backgrounds; 
    Rectangle exitZone;
};

// --- GLOBAL STATE (Wrapped in a struct for cleanliness) ---
struct GameState {
    GameScreen currentScreen = LOGO;
    int currentLevelIndex = 0;
    bool exitGame = false;
    int menuOption = 0; // Shared for Title and Pause
    int framesCounter = 0;

    int burnTimer = 0;
    
    // Player State
    Player player = {0};
    
    // Level Data
    std::vector<Level> levels;
    
    // Visuals
    StarDonutState donutState;
    CircularBuffer particleSystem;
    RenderTexture2D lightLayer;
    bool isPlayerBurning = false;
};

// --- HELPER FUNCTIONS FOR INITIALIZATION ---

std::vector<Level> InitLevels() {
    std::vector<Level> levels;

    // LEVEL 1
    Level lvl1;
    lvl1.backgrounds = std::make_tuple(LoadTexture("level1_daybackground.jpg"), LoadTexture("level1_nightbackground.jpg"));
    lvl1.spawnPoint = {50, (float)(SCREEN_HEIGHT - 100)};
    lvl1.platforms = {
        {{400, 600, 100, 10}, basic},
        {{600, 500, 100, 10}, basic},
        {{0, (float)(SCREEN_HEIGHT - 50), (float)SCREEN_WIDTH, 50}, basic}};
    lvl1.enemy = {lvl1.platforms[0].position.x + lvl1.platforms[0].position.width / 2, lvl1.platforms[0].position.y, {30, 30}, false, 0};
    lvl1.clouds = {
        {{100, 250, 200, 40}, 150.0f, 50, 600, true},
        {{700, 350, 250, 40}, 100.0f, 400, 900, false}};
    lvl1.sunPosition = {500.0f, -50.0f};
    levels.push_back(lvl1);

    // LEVEL 2
    Level lvl2;
    lvl2.spawnPoint = {50, 200};
    lvl2.platforms = {
        {{50, 250, 100, 10}, basic},
        {{300, 550, 100, 10}, basic},
        {{500, 450, 100, 10}, basic},
        {{700, 350, 100, 10}, basic},
        {{0, (float)(SCREEN_HEIGHT - 50), (float)SCREEN_WIDTH, 50}, basic}};
    lvl2.enemy = {{lvl2.platforms[1].position.x + lvl2.platforms[1].position.width / 2, lvl2.platforms[1].position.y}, {30, 30}, false, 1};
    lvl2.clouds = {
        {{200, 150, 180, 40}, 120.0f, 100, 500, true},
        {{800, 300, 220, 40}, 90.0f, 600, 1000, false},
        {{400, 100, 150, 40}, 180.0f, 300, 800, true}};
    lvl2.sunPosition = {800.0f, -50.0f};
    levels.push_back(lvl2);

    // LEVEL 3
    Level lvl3;
    lvl3.spawnPoint = {100, 600};
    lvl3.platforms = {
        {{100, 650, 150, 10}, basic},
        {{400, 600, 100, 10}, basic},
        {{600, 500, 100, 10}, basic},
        {{800, 400, 100, 10}, basic},
        {{1000, 300, 100, 10}, basic},
        {{0, (float)(SCREEN_HEIGHT + 200), (float)SCREEN_WIDTH, 50}, basic}};
    lvl3.enemy = {{lvl3.platforms[2].position.x + lvl3.platforms[2].position.width / 2, lvl3.platforms[2].position.y}, {30, 30}, false, 2};
    lvl3.clouds = {{{300, 200, 150, 40}, 200.0f, 200, 900, true}, {{600, 400, 150, 40}, 200.0f, 300, 1000, false}};
    lvl3.sunPosition = {200.0f, -50.0f};
    levels.push_back(lvl3);


    Level lvl4;
    lvl4.spawnPoint = {100, 600};
    lvl4.platforms = {
        {{100, 650, 1050, 20}, basic},
        {{300, 650, 150, 20}, basic},
        {{600, 600, 100, 10}, basic},
        {{800, 400, 100, 10}, basic},
        {{1000, 300, 100, 10}, basic},
        {{0, (float)(SCREEN_HEIGHT - 50), (float)SCREEN_WIDTH, 50}, basic}};
    //lvl4.enemy = {{lvl4.platforms[2].position.x + lvl4.platforms[2].position.width / 2, lvl4.platforms[2].position.y}, {30, 30}, false, 3};
    lvl4.clouds = {{{300, 200, 150, 40}, 200.0f, 200, 900, true}, {{600, 400, 150, 40}, 200.0f, 300, 1000, false}};
    lvl4.sunPosition = {256.0f, -55.5f};
    lvl4.exitZone = {1000, 600, 200, 200};
    levels.push_back(lvl4);

    return levels;
}

void ResetPlayer(GameState& game) {
    if (game.currentLevelIndex < (int)game.levels.size()) {
        game.player.position = game.levels[game.currentLevelIndex].spawnPoint;
        game.player.speed = 0;
        game.player.healthPoints = 5;
        game.player.toggleCooldown = 2.0f;
    }
}

// --- LOGIC UPDATES ---

void UpdateMenu(GameState& game, bool isPauseMenu) {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) game.menuOption++;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) game.menuOption--;

    if (game.menuOption < 0) game.menuOption = 1;
    if (game.menuOption > 1) game.menuOption = 0;

    if (IsKeyPressed(KEY_ENTER)) {
        if (isPauseMenu) {
            if (game.menuOption == 0) game.currentScreen = GAMEPLAY;        // RESUME
            else if (game.menuOption == 1) {                                // QUIT TO TITLE
                game.currentScreen = TITLE;
                game.menuOption = 0;
            }
        } else { // Title Menu
            if (game.menuOption == 0) game.currentScreen = STORY;           // PLAY
            else if (game.menuOption == 1) game.exitGame = true;            // QUIT
        }
    }
}

void UpdateGameplay(GameState& game, float delta) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        game.currentScreen = PAUSE;
        game.menuOption = 0;
        return;
    }

    // Prepare Level Data
    Level& currentLvlData = game.levels[game.currentLevelIndex];

    // Check Burning logic (1 HP per 60 frames)
    if (game.isPlayerBurning) {
        game.burnTimer++;
        if (game.burnTimer >= 60) {
            game.player.healthPoints -= 1;
            game.burnTimer = 0;
            std::cout << "Player burned! HP: " << game.player.healthPoints << std::endl;
        }
    } else {
        //game.burnTimer = 0; // Optional: Reset timer if they find shade
    }
    // Check for specific death condition if needed, or rely on player update
    if (game.player.healthPoints <= 0) {
        // Handle death (e.g., reset level or game over)
        ResetPlayer(game);
    }

    // 1. Update Player
    updatePlayer(game.player, currentLvlData.platforms.data(), (int)currentLvlData.platforms.size(), currentLvlData.enemy, delta, currentLvlData.spawnPoint);

    // 2. Update Enemy
    //if (!currentLvlData.platforms.empty()) {
    //    int pi = currentLvlData.enemy.patrolPlatformIndex;
    //    updateEnemy(currentLvlData.enemy, currentLvlData.platforms[pi].position, delta);
    //}
    
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
            if (CheckCollisionRecs({game.player.position.x, game.player.position.y, 1, 1}, cloud.rect)) {
                 // Simple overlap check often fails for "feet", prefer checking feet explicitly:
                 float feetY = game.player.position.y;
                 if (feetY >= cloud.rect.y && feetY <= cloud.rect.y + 15.0f &&
                     game.player.position.x >= cloud.rect.x && game.player.position.x <= cloud.rect.x + cloud.rect.width) {
                        
                     game.player.speed = 0.0f;
                     game.player.position.y = cloud.rect.y;
                     game.player.canJump = true;
                     game.player.position.x += (cloud.movingRight ? moveAmount : -moveAmount);
                 }
            }
        }
    }

    // 4. Level Transition Test
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
}

// --- DRAWING FUNCTIONS ---

void DrawLighting(GameState& game, Level& level) {
    // 3D Shadow Casting Logic
    Vector2 sunPos = level.sunPosition;
    BoundingBox playerBox3D = {
        (Vector3){game.player.position.x - game.player.size.x / 2, game.player.position.y - game.player.size.y, -10},
        (Vector3){game.player.position.x + game.player.size.x / 2, game.player.position.y, 10}
    };

    BeginTextureMode(game.lightLayer);
    ClearBackground(BLANK);

    bool anyRayHitPlayer = false;

    // Raycast loop
    for (int x = -200; x <= SCREEN_WIDTH + 200; x += 40) {
        Vector2 targetPos = {(float)x, (float)SCREEN_HEIGHT};
        Vector3 sunOrigin3D = {sunPos.x, sunPos.y, 0};
        Vector3 rayDir = {targetPos.x - sunPos.x, targetPos.y - sunPos.y, 0};
        float len = sqrt(rayDir.x * rayDir.x + rayDir.y * rayDir.y);
        if (len == 0) continue;
        rayDir.x /= len; rayDir.y /= len;
        
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
        for (const auto& plat : level.platforms) {
            BoundingBox platBox = {(Vector3){plat.position.x, plat.position.y, -10}, (Vector3){plat.position.x + plat.position.width, plat.position.y + plat.position.height, 10}};
            RayCollision platHit = GetRayCollisionBox(currentRay, platBox);
            if (platHit.hit && platHit.distance < nearestDist) {
                nearestDist = platHit.distance;
                hitPlayer = false;
            }
        }

        // Check Clouds
        for (const auto& cloud : level.clouds) {
             BoundingBox cloudBox = {(Vector3){cloud.rect.x, cloud.rect.y, -10}, (Vector3){cloud.rect.x + cloud.rect.width, cloud.rect.y + cloud.rect.height, 10}};
             RayCollision cloudHit = GetRayCollisionBox(currentRay, cloudBox);
             if (cloudHit.hit && cloudHit.distance < nearestDist) {
                 nearestDist = cloudHit.distance;
                 hitPlayer = false;
             }
        }

        Vector2 endPos = {sunPos.x + rayDir.x * (nearestDist + 20.0f), sunPos.y + rayDir.y * (nearestDist + 20.0f)};
        if (hitPlayer) {
             rayColor = RED;
             anyRayHitPlayer = true;
             endPos = {sunPos.x + rayDir.x * nearestDist, sunPos.y + rayDir.y * nearestDist};
        }
        DrawLineEx(sunPos, endPos, 40.0f, rayColor);
        if(hitPlayer) DrawCircleV(endPos, 5, RED);
    }
    EndTextureMode();

    game.isPlayerBurning = anyRayHitPlayer;

    // Draw the texture to screen
    DrawTextureRec(game.lightLayer.texture, (Rectangle){0, 0, (float)SCREEN_WIDTH, (float)-SCREEN_HEIGHT}, (Vector2){0, 0}, Fade(WHITE, 0.2f));
}

void DrawGameplay(GameState& game) {
    Level& currentLvlData = game.levels[game.currentLevelIndex];

    // Background
    Texture2D bg = std::get<0>(currentLvlData.backgrounds);
    if (bg.id != 0) DrawTexture(bg, 0, 0, WHITE);
    else DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, PURPLE);

    // Platforms
    for (const auto& plat : currentLvlData.platforms) {
        DrawRectangleRec(plat.position, GRAY);
    }

    // Clouds
    for (const auto& cloud : currentLvlData.clouds) {
        DrawRectangleRec(cloud.rect, Fade(SKYBLUE, 0.9f));
        DrawRectangleLinesEx(cloud.rect, 2, WHITE);
    }

    // Player (Primitive drawing, can be replaced with sprite logic)
    Vector2 playerPosVisual = {game.player.position.x - game.player.size.x / 2, game.player.position.y - game.player.size.y};
    DrawRectangleV(playerPosVisual, game.player.size, BLUE);
    DrawRectangleLinesEx((Rectangle){playerPosVisual.x, playerPosVisual.y, game.player.size.x, game.player.size.y}, 2.0f, BLACK);

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
    // Note: Enemy drawing is likely inside the enemy struct logic or missing from original code explicitly, adding basic rect:
    // Assuming enemy was drawn previously or not crucial for refactor demo, skipping explicit draw call if not found in original snippet logic.
    // Draw Enemy
    if (!currentLvlData.platforms.empty()) {
        int pi = currentLvlData.enemy.patrolPlatformIndex;
        // We need 'delta' here. Since DrawGameplay doesn't typically take delta, 
        // strictly speaking we should pass it, or just use GetFrameTime() directly here.
        updateEnemy(currentLvlData.enemy, currentLvlData.platforms[pi].position, GetFrameTime());
    }
    // Particles
    UpdateParticles(&game.particleSystem);
    if (game.isPlayerBurning) {
        Vector2 center = {game.player.position.x, game.player.position.y - game.player.size.y / 2};
        EmitParticle(&game.particleSystem, center, FIRE);
        EmitParticle(&game.particleSystem, center, FIRE);
        DrawText("BURNING!", SCREEN_WIDTH / 2, 50, 40, RED);
    } else {
        DrawText("SAFE", SCREEN_WIDTH / 2, 50, 40, GREEN);
    }
    DrawParticles(&game.particleSystem);

    // Debug
    DrawText(TextFormat("Level: %d", game.currentLevelIndex + 1), 10, 10, 20, WHITE);
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

    GameState game;
    game.levels = InitLevels();
    game.player.size = {40, 80};
    ResetPlayer(game);
    
    InitParticles();
    game.particleSystem = InitParticles(); 
    InitStarDonut(&game.donutState, SCREEN_WIDTH, SCREEN_HEIGHT);
    game.lightLayer = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

    while (!WindowShouldClose() && !game.exitGame) {
        float delta = GetFrameTime();

        // --- UPDATE ---
        switch (game.currentScreen) {
            case LOGO:
                game.framesCounter++;
                if (game.framesCounter > 120) game.currentScreen = TITLE;
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
            case ENDING:
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) game.currentScreen = CREDIT;
                break;
            case CREDIT:
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) game.currentScreen = TITLE;
                break;
            default: break;
        }

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

    UnloadRenderTexture(game.lightLayer);
    CloseWindow();
    return 0;
}