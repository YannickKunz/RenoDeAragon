#include "game.h"
#include "constants.h"
#include "resource_dir.h"
#include <cmath>
#include <iostream>

Game::Game() { Init(); }

Game::~Game() { Cleanup(); }

void Game::Init() {
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LVLUP Game Jam 2026");
  SetExitKey(KEY_NULL);
  SearchAndSetResourceDir("resources");

  InitAudioDevice();

  currentScreen = LOGO; // Start with LOGO as in main
  currentLevelIndex = 0;
  exitGame = false;
  menuOption = 0;
  framesCounter = 0;
  burnTimer = 0;
  playerToggle = true;
  musicToggle = !playerToggle;
  isPlayerBurning = false;

  // Load Textures
  // Load Textures
  // Player and Spider are sprite sheets, so we CANNOT AutoCrop them (breaks
  // frame alignment). We will manually trim the padding in the draw functions.
  player.texture =
      LoadTexture("images:anims/walkingDayCharAnimationSpreadsheet.png");

  Image gameOverImg = LoadImage("images:anims/gameOverScreen.png");
  ImageResize(&gameOverImg, SCREEN_WIDTH, SCREEN_HEIGHT);
  texGameOver = LoadTextureFromImage(gameOverImg);
  texGameOver2 = texGameOver;
  UnloadImage(gameOverImg);

  texSpider = LoadTexture("images:anims/spiderMoveSpreadsheet.png");
  texRoach = LoadAndCrop(
      "images:anims/roach.png"); // Roach is single sprite, AutoCrop OK

  Image introImg = LoadImage("images:anims/introScreenWithBackground.png");
  ImageResize(&introImg, SCREEN_WIDTH, SCREEN_HEIGHT);
  texIntroScreen = LoadTextureFromImage(introImg);
  UnloadImage(introImg);

  Image titleImg = LoadImage("images:anims/introImage.png");
  ImageResize(&titleImg, SCREEN_WIDTH, SCREEN_HEIGHT);
  texIntroImage = LoadTextureFromImage(titleImg);
  UnloadImage(titleImg);

  texDeathPlayer = LoadAndCrop("images:anims/deathPlayerPot.png");
  texExit = LoadAndCrop("images:anims/dayWaterPot.png");
  texExitNight = LoadAndCrop("images:anims/nightWaterPot.png");

  if (texExit.id == 0) {
    std::cout << "ERROR: Could not load 'dayWaterPot.png'" << std::endl;
  }

  // Load Sounds
  sndJump = LoadSound("music:sounds/jump_sound.wav");
  sndDeath = LoadSound("music:sounds/death_sound.wav");
  sndWalk = LoadSound("music:sounds/walk_sound.wav");
  sndBurn = LoadSound("music:sounds/burn_sound.wav");
  sndWater = LoadSound("music:sounds/watering_can.wav");

  // Initialize Game Objects
  levels = InitLevels();

  // Update player for animation
  player.frameRec = {0.0f, 0.0f, (float)player.texture.width / 6,
                     (float)player.texture.height}; // Assuming 6 frames
  player.currentFrame = 0;
  player.framesCounter = 0;
  player.framesSpeed = 8;

  // Set size based on frame size, maybe scale it down a bit? the spreadsheet
  // might be large
  player.size = {60, 100}; // Reduced from 80, 140 to be smaller
  ResetPlayer();

  particleSystem = InitParticles();
  // InitStarDonut(&donutState, SCREEN_WIDTH, SCREEN_HEIGHT);
  lightLayer = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

  // Load Music
  if (!levels.empty()) {
    music.first = LoadMusicStream(levels[0].musicPath.first.c_str());
    music.second = LoadMusicStream(levels[0].musicPath.second.c_str());
  }

  SelectPlayMusic();
}

void Game::Cleanup() {
  for (auto &level : levels) {
    level.Unload();
  }
  UnloadRenderTexture(lightLayer);
  // UnloadTexture(texBasic);
  // UnloadTexture(texMushroom);
  // UnloadTexture(texFlower);
  UnloadTexture(player.texture);
  UnloadTexture(texSpider);
  UnloadTexture(texRoach);
  UnloadTexture(texGameOver);
  UnloadTexture(texGameOver2);
  UnloadTexture(texExit);
  UnloadTexture(texExitNight);
  UnloadTexture(texIntroScreen);
  UnloadTexture(texIntroImage);
  UnloadTexture(texDeathPlayer);

  UnloadMusicStream(music.first);
  UnloadMusicStream(music.second);

  UnloadSound(sndJump);
  UnloadSound(sndDeath);
  UnloadSound(sndWalk);
  UnloadSound(sndBurn);
  UnloadSound(sndWater);

  FreeParticles(&particleSystem); // If such function exists, otherwise it might
                                  // just free memory
  CloseAudioDevice();
  CloseWindow();
}

void Game::Run() {
  while (!WindowShouldClose() && !exitGame) {
    float delta = GetFrameTime();
    Update(delta);
    Draw();
  }
}

void Game::Update(float delta) {
  UpdateMusicStream(music.first);
  UpdateMusicStream(music.second);

  switch (currentScreen) {
  case LOGO:
    framesCounter++;
    if (framesCounter > 120) // 2 seconds at 60fps
      currentScreen = TITLE;
    break;
  case TITLE:
    UpdateMenu(false);
    break;
  case STORY:
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
      ResetPlayer();
      currentScreen = GAMEPLAY;
    }
    break;
  case GAMEPLAY:
    UpdateGameplay(delta);
    break;
  case PAUSE:
    UpdateMenu(true);
    if (IsKeyPressed(KEY_ESCAPE))
      currentScreen = GAMEPLAY;
    break;
  case GAME_OVER:
    if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER)) {
      ResetPlayer();
      currentScreen = GAMEPLAY;
    }
    break;
  case ENDING:
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
      currentScreen = CREDIT;
    break;
  case CREDIT:
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
      currentScreen = TITLE;
    break;
  default:
    break;
  }

  SelectPlayMusic();
}

void Game::Draw() {
  BeginDrawing();
  ClearBackground(BLACK);

  switch (currentScreen) {
  case LOGO:
    // Logic moved here to handle timer update during Draw is okay, usually
    // Update handles logic But since we are inside Draw switch, we can just
    // Draw. Update() handles state switch. User wants
    // "introScreenWithBackground.png should be played and be full screensized
    // before the title screen and be displayed for like 2 seconds" We handle
    // the timer in Update(), here just draw it full screen.
    DrawTexturePro(texIntroScreen,
                   (Rectangle){0, 0, (float)texIntroScreen.width,
                               (float)texIntroScreen.height},
                   (Rectangle){0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
                   (Vector2){0, 0}, 0.0f, WHITE);
    break;
  case TITLE:
    // User wants "intro_image.png should be player with the title screen at the
    // top" "we can select start game or exit"
    DrawTexturePro(texIntroImage,
                   (Rectangle){0, 0, (float)texIntroImage.width,
                               (float)texIntroImage.height},
                   (Rectangle){0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
                   (Vector2){0, 0}, 0.0f, WHITE);

    // Draw Menu Text Overlay
    DrawMenu("", "PLAY GAME", "QUIT", menuOption); // Title inside image?
    break;
  case STORY: {
    const char *storyText =
        "Dans un pot vit petit Jasmin.\n\n"
        "Petit Jasmin vit sa meilleur vie, arrosé régulièrement par un chill "
        "dude.\n"
        "Mais voilà l\'été arrivé, le soleil tape fort et la soif se fait "
        "ressentir.\n"
        "Depuis plusieurs jours, Petit Jasmin n\'a pas été arrosé.\n"
        "Il semblerait que plus personne ne soit là pour s\'en occuper.\n\n"
        "Il va falloir sortir de son pot et chercher de l\'eau,\n"
        "sous peine de mourir dans d\'atroce souffrance,\n"
        "petit à petit sous la chaleur écrasante du soleil cuisant,\n"
        "terrible destin fatale.\n\n"
        "Mais Petit Jasmin n\'est jamais sortie de son pot\n"
        "(parce que les plantes sa sort pas des pots),\n"
        "mais il va falloir défier toute logique pour chercher de l\'eau,\n"
        "mais attention, hors de son pot,\n"
        "Petit Jasmin est encore plus sensible au soleil !";

    int fontSize = 30; // Reduced font size to fit text
    int textWidth = MeasureText(storyText, fontSize);
    // MeasureText only measures single line width (of the longest line usually
    // in simple impl, but Raylib MeasureText might not handle \n correctly for
    // width). Let's just draw it centered manually or left-aligned with
    // padding.

    DrawText(storyText, 50, 50, fontSize, WHITE);

    DrawText("PRESS ENTER TO START",
             SCREEN_WIDTH - MeasureText("PRESS ENTER TO START", 20) - 50,
             SCREEN_HEIGHT - 50, 20, YELLOW);
  } break;
  case GAMEPLAY:
    DrawGameplay();
    break;
  case PAUSE:
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8f));
    DrawMenu("PAUSED", "RESUME", "QUIT TO TITLE", menuOption);
    break;
  case GAME_OVER: {
    // User wants "gameOverScreen.png should be displayed when the player dies"
    DrawTexturePro(
        texGameOver,
        (Rectangle){0, 0, (float)texGameOver.width, (float)texGameOver.height},
        (Rectangle){0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
        (Vector2){0, 0}, 0.0f, WHITE);

    // "option to restart or press R to restart"
    const char *text = "PRESS 'R' TO RESTART";
    DrawText(text, SCREEN_WIDTH / 2 - MeasureText(text, 30) / 2,
             SCREEN_HEIGHT - 100, 30, WHITE);
  } break;
  case ENDING:
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLUE);
    DrawText("ENDING SCREEN",
             SCREEN_WIDTH / 2 - MeasureText("ENDING SCREEN", 40) / 2,
             SCREEN_HEIGHT / 4, 40, LIGHTGRAY);
    DrawText("PRESS ENTER",
             SCREEN_WIDTH / 2 - MeasureText("PRESS ENTER", 20) / 2,
             SCREEN_HEIGHT - 100, 20, LIGHTGRAY);
    break;
  case CREDIT:
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKGRAY);
    DrawText("CREDITS", SCREEN_WIDTH / 2 - MeasureText("CREDITS", 40) / 2,
             SCREEN_HEIGHT / 4, 40, LIGHTGRAY);
    DrawText("Developed by Reino de Aragon",
             SCREEN_WIDTH / 2 -
                 MeasureText("Developed by Reino de Aragon", 20) / 2,
             SCREEN_HEIGHT / 2, 20, LIGHTGRAY);
    break;
  default:
    break;
  }

  EndDrawing();
}

void Game::ResetPlayer() {
  if (currentLevelIndex < (int)levels.size()) {
    player.position = levels[currentLevelIndex].spawnPoint;
    player.speed = 0;
    player.healthPoints = 3;
    player.toggleCooldown = 2.0f;
  }
}

void Game::SelectPlayMusic() {
  if (musicToggle != playerToggle) {
    musicToggle = playerToggle;
    if (!playerToggle) {
      StopMusicStream(music.first);
      PlayMusicStream(music.second);
    } else {
      StopMusicStream(music.second);
      PlayMusicStream(music.first);
    }
  }
}

void Game::ChangeMusicStreams() {
  StopMusicStream(music.first);
  StopMusicStream(music.second);

  Level &level = levels[currentLevelIndex];
  music.first = LoadMusicStream(level.musicPath.first.c_str());
  music.second = LoadMusicStream(level.musicPath.second.c_str());
  musicToggle = !playerToggle;
}

void Game::UpdateMenu(bool isPauseMenu) {
  if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
    menuOption++;
  if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
    menuOption--;

  if (menuOption < 0)
    menuOption = 1;
  if (menuOption > 1)
    menuOption = 0;

  if (IsKeyPressed(KEY_ENTER)) {
    if (isPauseMenu) {
      if (menuOption == 0)
        currentScreen = GAMEPLAY; // RESUME
      else if (menuOption == 1) { // QUIT TO TITLE
        currentScreen = TITLE;
        menuOption = 0;
      }
    } else { // Title Menu
      if (menuOption == 0)
        currentScreen = STORY; // PLAY
      else if (menuOption == 1)
        exitGame = true; // QUIT
    }
  }
}

void Game::UpdateGameplay(float delta) {
  if (IsKeyPressed(KEY_ESCAPE)) {
    currentScreen = PAUSE;
    menuOption = 0;
    return;
  }

  Level &currentLvlData = levels[currentLevelIndex];

  // Check Burning logic (1 HP per 60 frames)
  if (playerToggle && isPlayerBurning) {
    burnTimer++;
    if (burnTimer % 15 == 0) {
      Vector2 firePos = {player.position.x,
                         player.position.y - player.size.y / 2};
      EmitParticle(&particleSystem, firePos, FIRE);
    }
    if (burnTimer >= 60) {
      player.healthPoints -= 1;
      burnTimer = 0;
    }
  }

  if (player.healthPoints <= 0) {
    currentScreen = GAME_OVER;
  }

  // Update Player
  updatePlayer(player, currentLvlData.platforms, currentLvlData.enemies, delta,
               currentLvlData.spawnPoint, playerToggle, sndJump, sndWalk);

  // Update Clouds & Player Cloud Riding
  for (auto &cloud : currentLvlData.clouds) {
    UpdateCloud(cloud, delta); // Using the new function

    // Riding logic
    if (player.speed >= 0) {
      Rectangle playerFeet = {player.position.x, player.position.y, 1,
                              1}; // Rough approximation

      // Re-implementing the logic from main.cpp
      if (CheckCollisionRecs(playerFeet, cloud.rect)) {
        // But wait, the original logic checked specifically:
        float feetY = player.position.y;
        if (feetY >= cloud.rect.y && feetY <= cloud.rect.y + 15.0f &&
            player.position.x >= cloud.rect.x &&
            player.position.x <= cloud.rect.x + cloud.rect.width) {

          player.speed = 0.0f;
          player.position.y = cloud.rect.y;
          player.canJump = true;
          float moveAmount = cloud.speed * delta;
          player.position.x += (cloud.movingRight ? moveAmount : -moveAmount);
        }
      }
    }
  }

  // Level Transition Test
  int prevLevelIdx = currentLevelIndex;

  Rectangle playerRect = {player.position.x - player.size.x / 2,
                          player.position.y - player.size.y, player.size.x,
                          player.size.y};

  if (CheckCollisionRecs(playerRect, currentLvlData.exitZone)) {
    currentLevelIndex++;
    if (currentLevelIndex >= (int)levels.size()) {
      currentScreen = ENDING;
      currentLevelIndex = 0;
    } else {
      ResetPlayer();
    }
  }

  if (prevLevelIdx != currentLevelIndex) {
    playerToggle = levels[currentLevelIndex].isDay;
    ChangeMusicStreams();
  }
}

void Game::DrawLighting(Level &level) {
  if (!playerToggle) {
    isPlayerBurning = false;
    BeginTextureMode(lightLayer);
    ClearBackground(BLANK);
    EndTextureMode();
    return;
  }

  Vector2 sunPos = level.sunPosition;
  BoundingBox playerBox3D = {
      (Vector3){player.position.x - player.size.x / 2,
                player.position.y - player.size.y, -10},
      (Vector3){player.position.x + player.size.x / 2, player.position.y, 10}};

  BeginTextureMode(lightLayer);
  ClearBackground(BLANK);

  bool anyRayHitPlayer = false;

  for (int x = -200; x <= SCREEN_WIDTH + 200; x += 30) {
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

    RayCollision playerHit = GetRayCollisionBox(currentRay, playerBox3D);
    if (playerHit.hit && playerHit.distance < nearestDist) {
      nearestDist = playerHit.distance;
      hitPlayer = true;
    }

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
    DrawLineEx(sunPos, endPos, 15.0f, rayColor);
    if (hitPlayer)
      DrawCircleV(endPos, 5, RED);
  }
  EndTextureMode();

  isPlayerBurning = anyRayHitPlayer;

  DrawTextureRec(lightLayer.texture,
                 (Rectangle){0, 0, (float)SCREEN_WIDTH, (float)-SCREEN_HEIGHT},
                 (Vector2){0, 0}, Fade(WHITE, 0.15f));
}

void Game::DrawGameplay() {
  Level &currentLvlData = levels[currentLevelIndex];

  Texture2D bg = playerToggle ? std::get<0>(currentLvlData.backgrounds)
                              : std::get<1>(currentLvlData.backgrounds);
  if (bg.id != 0) {
    DrawTexture(bg, 0, 0, WHITE);
  } else {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, PURPLE);
  }

  for (const auto &plat : currentLvlData.platforms) {
    drawPlatform(plat, currentLvlData.texMushroom, currentLvlData.texFlower,
                 currentLvlData.texBasic, playerToggle);
  }

  Texture2D currentExitTex = playerToggle ? texExit : texExitNight;
  if (currentExitTex.id != 0) {
    Rectangle sourceRec = {0.0f, 0.0f, (float)currentExitTex.width,
                           (float)currentExitTex.height};
    // Scale based on HEIGHT to ensure it stands out
    // Texture is now cropped, so it should be tight.
    float targetHeight = currentLvlData.exitZone.height *
                         0.9f; // 0.9x height (User said 1.5 was too big)
    float drawHeight = targetHeight;
    float drawWidth = (float)currentExitTex.width *
                      (drawHeight / (float)currentExitTex.height);

    // Center it in the exit box
    Rectangle drawRect = {
        currentLvlData.exitZone.x +
            (currentLvlData.exitZone.width - drawWidth) /
                2, // Center horizontally
        currentLvlData.exitZone.y +
            (currentLvlData.exitZone.height - drawHeight), // Align bottom
        drawWidth, drawHeight};

    DrawTexturePro(currentExitTex, sourceRec, drawRect, {0, 0}, 0.0f, WHITE);
  } else {
    DrawRectangleRec(currentLvlData.exitZone, Fade(GREEN, 0.3f));
    DrawRectangleLinesEx(currentLvlData.exitZone, 2.0f, GREEN);
    DrawText("EXIT", (int)currentLvlData.exitZone.x + 5,
             (int)currentLvlData.exitZone.y - 20, 20, GREEN);
  }

  for (const auto &cloud : currentLvlData.clouds) {
    DrawRectangleRec(cloud.rect, Fade(SKYBLUE, 0.9f));
    DrawRectangleLinesEx(cloud.rect, 2, WHITE);
  }

  drawPlayer(player);

  // HP Bar
  Vector2 playerPosVisual = {player.position.x - player.size.x / 2,
                             player.position.y - player.size.y};
  float barWidth = 6.0f;
  float barHeight = 40.0f; // Slightly smaller
  float barX = playerPosVisual.x + player.size.x + 4.0f;
  float barY = playerPosVisual.y + (player.size.y - barHeight) / 2.0f;

  DrawRectangle(barX, barY, barWidth, barHeight, Fade(BLACK, 0.5f));
  DrawRectangleLines(barX, barY, barWidth, barHeight, BLACK);

  float maxHP = 5.0f; // Was 5.0f in main.cpp
  float hpPct = (float)player.healthPoints / maxHP;
  if (hpPct < 0.0f)
    hpPct = 0.0f;

  float fillHeight = barHeight * hpPct;
  float fillY = barY + (barHeight - fillHeight);

  Color barColor = GREEN;
  if (player.healthPoints <= 2)
    barColor = RED;
  else if (player.healthPoints <= 4)
    barColor = ORANGE;

  DrawRectangle(barX, fillY, barWidth, fillHeight, barColor);

  DrawLighting(currentLvlData);

  DrawCircleV(currentLvlData.sunPosition, 40, YELLOW);
  if (isEnemyActive(playerToggle)) {
    for (Enemy &enemy : currentLvlData.enemies) {
      int pi = enemy.patrolPlatformIndex;
      Texture2D currentTex = (enemy.type == spider) ? texSpider : texRoach;
      updateEnemy(enemy, currentLvlData.platforms[pi].position, GetFrameTime(),
                  currentTex);
    }
  }

  // Draw Player Hitbox (Debug)
  Rectangle playerRect = {player.position.x - player.size.x / 2,
                          player.position.y - player.size.y, player.size.x,
                          player.size.y};
  DrawRectangleLinesEx(playerRect, 2.0f, GREEN);

  // Exit Zone Debug
  DrawRectangleLinesEx(currentLvlData.exitZone, 3.0f, MAGENTA);

  UpdateParticles(&particleSystem);
  DrawParticles(&particleSystem);
}

void Game::DrawMenu(const char *title, const char *opt1, const char *opt2,
                    int selection) {
  DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKGREEN);
  DrawText(title, SCREEN_WIDTH / 2 - MeasureText(title, 30) / 2,
           SCREEN_HEIGHT / 3, 30, YELLOW);

  Color color1 = (selection == 0) ? YELLOW : WHITE;
  Color color2 = (selection == 1) ? YELLOW : WHITE;

  DrawText(opt1, SCREEN_WIDTH / 2 - MeasureText(opt1, 30) / 2,
           SCREEN_HEIGHT / 2, 30, color1);
  DrawText(opt2, SCREEN_WIDTH / 2 - MeasureText(opt2, 30) / 2,
           SCREEN_HEIGHT / 2 + 50, 30, color2);

  int yPos = (selection == 0) ? SCREEN_HEIGHT / 2 : SCREEN_HEIGHT / 2 + 50;
  const char *selectedText = (selection == 0) ? opt1 : opt2;
  DrawText(">", SCREEN_WIDTH / 2 - MeasureText(selectedText, 30) / 2 - 30, yPos,
           30, YELLOW);
}
