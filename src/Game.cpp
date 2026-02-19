#include "Game.h"
#include "entities/Roach.h"
#include "entities/Spider.h"
#include <tuple>

// Helper: Check collision between a Line and a Rectangle
bool CheckCollisionLineRect(Vector2 start, Vector2 end, Rectangle rect) {
  // Check if either point is INSIDE
  if (CheckCollisionPointRec(start, rect) || CheckCollisionPointRec(end, rect))
    return true;

  // Check against 4 edges
  Vector2 p1 = {rect.x, rect.y};
  Vector2 p2 = {rect.x + rect.width, rect.y};
  Vector2 p3 = {rect.x + rect.width, rect.y + rect.height};
  Vector2 p4 = {rect.x, rect.y + rect.height};

  Vector2 collisionPoint;
  if (CheckCollisionLines(start, end, p1, p2, &collisionPoint))
    return true;
  if (CheckCollisionLines(start, end, p2, p3, &collisionPoint))
    return true;
  if (CheckCollisionLines(start, end, p3, p4, &collisionPoint))
    return true;
  if (CheckCollisionLines(start, end, p4, p1, &collisionPoint))
    return true;

  return false;
}

Game::Game() {
  isDayTime = true;
  debugMode = false;
  currentLevelIndex = 0;
  titleMusicLoaded = false;
  currentPlayingMusic = nullptr;
  isMusicPlaying = false;
  isUnloaded = false;
  sunHintShown = false;
  sunHintTimer = 0.0f;

  // Settings defaults
  masterVolume = 1.0f;
  musicVolume = 0.7f;
  sfxVolume = 1.0f;
  isFullscreen = false;
  selectedResIndex = 2; // 1280x800
  settingsSelection = 0;
  previousScreen = TITLE;

  // Initialize animation state
  flowerAnimFrameCount = 6;
  flowerAnimCurrentFrame = 0;
  flowerAnimTimer = 0.0f;
  flowerAnimSpeed = 0.15f;
}

Game::~Game() {
  if (!isUnloaded) {
    Unload();
  }
}

void Game::UnloadCurrentLevelEntities() {
  for (auto *enemy : currentEnemies) {
    delete enemy;
  }
  currentEnemies.clear();
}

void Game::Init() {
  isDayTime = true; // Reset to Day on Init

  // --- LOAD TEXTURES ---

  // Player textures
  playerIdleTex = LoadTexture("assets/sprites/dayCharacter.png");
  playerWalkSheet =
      LoadTexture("assets/sprites/walkingDayCharAnimationSpreadsheet.png");
  playerDeathTex = LoadTexture("assets/sprites/deathPlayerPot.png");

  // Set player textures
  player.sprite = playerIdleTex;
  player.textureLoaded = true;
  player.spritesheet = playerWalkSheet;
  player.frameCount = 6;
  player.frameSpeed = 0.1f;
  player.animated = true;
  player.deathSprite = playerDeathTex;
  player.deathSpriteLoaded = true;

  // Player size - images are now cropped to just the sprite content
  player.width = (float)Core::SCREEN_HEIGHT * 0.12f;
  player.height = (float)Core::SCREEN_HEIGHT * 0.12f;

  // Enemy textures
  roachTex = LoadTexture("assets/sprites/roach.png");
  spiderSheet = LoadTexture("assets/sprites/spiderMoveSpreadsheet.png");

  // UI/Screen textures
  introScreenTex = LoadTexture("assets/sprites/introScreenWithBackground.png");
  introImageTex = LoadTexture("assets/sprites/introImage.png");
  gameOverScreenTex = LoadTexture("assets/sprites/gameOverScreen.png");

  // Exit zone (watering can) textures
  waterPotDayTex = LoadTexture("assets/sprites/dayWaterPot.png");
  waterPotNightTex = LoadTexture("assets/sprites/nightWaterPot.png");

  // Platform textures
  flowerTex = LoadTexture("assets/sprites/flower.png");
  flowerAnimSheet =
      LoadTexture("assets/sprites/flowerAnimationSpreadsheet.png");
  mushroomDayTex = LoadTexture("assets/sprites/mushroomDayUpDown.png");
  mushroomNightTex = LoadTexture("assets/sprites/nightShroom.png");
  platformDayTex = LoadTexture("assets/sprites/PlatformTextureLevel1.png");
  platformNightTex =
      LoadTexture("assets/sprites/PlatformTextureLevel1Night.png");

  // --- LOAD AUDIO ---
  jumpSound = LoadSound("assets/audio/jump_sound.wav");
  walkSound = LoadSound("assets/audio/walk_sound.wav");
  burnSound = LoadSound("assets/audio/burn_sound.wav");
  deathSound = LoadSound("assets/audio/death_sound.wav");
  wateringCanSound = LoadSound("assets/audio/watering_can.wav");

  titleMusic = LoadMusicStream("assets/audio/titlescreenmusicmp3.mp3");
  titleMusicLoaded = true;

  // Helper lambda for relative coordinates
  auto RelWidth = [](float p) { return p * Core::SCREEN_WIDTH; };
  auto RelHeight = [](float p) { return p * Core::SCREEN_HEIGHT; };

  // --- LOAD LEVEL DATA ---

  // LEVEL 1
  Level lvl1;

  // Backgrounds (from images)
  Texture2D lvl1DayBg = LoadTexture("assets/sprites/level1day.png");
  Texture2D lvl1NightBg = LoadTexture("assets/sprites/level1night.jpg");
  lvl1.backgrounds = std::make_tuple(lvl1DayBg, lvl1NightBg);

  // Foregrounds
  lvl1.foregroundDay = LoadTexture("assets/sprites/foregroundDay.png");
  lvl1.foregroundNight = LoadTexture("assets/sprites/foregroundNight.png");
  lvl1.hasForeground = true;

  // Music
  lvl1.dayMusic = LoadMusicStream("assets/audio/lvlupjam_lvl1.wav");
  lvl1.nightMusic = LoadMusicStream("assets/audio/lvlupjam_lvl1_night.wav");
  lvl1.hasDayMusic = true;
  lvl1.hasNightMusic = true;

  levels.push_back(lvl1);

  // LEVEL 2
  Level lvl2;
  Texture2D lvl2DayBg = LoadTexture("assets/sprites/level2day.png");
  Texture2D lvl2NightBg = LoadTexture("assets/sprites/level2night.png");
  lvl2.backgrounds = std::make_tuple(lvl2DayBg, lvl2NightBg);

  // No foreground for level 2
  lvl2.hasForeground = false;

  // Music
  lvl2.dayMusic = LoadMusicStream("assets/audio/lvlupjam_lvl2.wav");
  lvl2.nightMusic = LoadMusicStream("assets/audio/lvlupjam_lvl2_night.wav");
  lvl2.hasDayMusic = true;
  lvl2.hasNightMusic = true;

  lvl2.isDay = false;
  levels.push_back(lvl2);

  // Initialize geometry for current resolution
  RebuildLevelGeometry();

  // Initial Load
  currentScreen = TITLE;
}

void Game::RebuildLevelGeometry() {
  float W = (float)Core::SCREEN_WIDTH;
  float H = (float)Core::SCREEN_HEIGHT;

  // Clear existing platforms and enemies for all levels
  for (auto &lvl : levels) {
    lvl.platforms.clear();
    lvl.enemies.clear();
  }

  // LEVEL 1 Geometry
  Level &lvl1 = levels[0];
  lvl1.spawnPoint = {W * 0.85f, H * 0.75f}; // Spawn on right side
  lvl1.platforms = {
      // Main Ground
      {{0, H * 0.9375f, W, H * 0.0625f}, basic},

      // Boundaries (Invisible) - All 4 sides
      {{-20, 0, 20, H}, invisible}, // Left Wall
      {{W, 0, 20, H}, invisible},   // Right Wall
      {{0, -20, W, 20}, invisible}, // Ceiling
      {{0, H, W, 20}, invisible},   // Floor (fallback)

      // Level Platforms (proportional)
      {{W * 0.57f, H * 0.5f, W * 0.43f, 30}, basic},
      {{0, H * 0.5f, W * 0.23f, 30}, basic},
      {{W * 0.31f, H * 0.84f, W * 0.08f, H * 0.125f}, mushroom},
  };

  // Sun: Top Center
  lvl1.sunPosition = {W / 2, 50.0f};
  lvl1.isDay = true;
  lvl1.exitZone = {50, H * 0.5f - 65, 100, 50};

  // LEVEL 2 Geometry
  Level &lvl2 = levels[1];
  lvl2.spawnPoint = {50, H * 0.75f};

  // Platforms (all proportional) - Lowered for easier difficulty
  lvl2.platforms = {
      // Ground
      {{0, H * 0.9375f, W, H * 0.0625f}, basic},

      // Boundaries - all 4 sides
      {{-20, 0, 20, H}, invisible},
      {{W, 0, 20, H}, invisible},
      {{0, -20, W, 20}, invisible},
      {{0, H, W, 20}, invisible},

      // Flower Platform (Lowered: 0.56 -> 0.65)
      {{W * 0.5f - 75, H * 0.65f, 150, 30}, flower},

      // Steps leading up
      {{W * 0.15f, H * 0.85f, 150, 30}, basic}, // Step 1
      {{W * 0.31f, H * 0.75f, 150, 30}, basic}, // Step 2

      // Right side high ground (Lowered: 0.5 -> 0.6)
      {{W * 0.69f, H * 0.6f, W * 0.23f, 30}, basic},

      // Bridging Platforms to Goal (Lowered ~0.15)
      {{W * 0.80f, H * 0.45f, 150, 30}, basic},
      {{W * 0.55f, H * 0.35f, 150, 30}, basic},
      {{W * 0.5f + 50, H * 0.25f, 150, 30}, basic},

      // Floating island high up (Goal) (Lowered: 0.05 -> 0.15)
      {{W * 0.5f - 200, H * 0.15f, 400, 30}, basic},
  };

  // Enemy (Stationary blocker on the way to the goal)
  float step2Y = H * 0.75f; // Matches Step 2
  float roachHeight = H * 0.25f;

  lvl2.enemies.push_back(
      {{W * 0.35f, step2Y - roachHeight}, {0, 0}, true, 5, roach});

  // Spider on Goal Island (Patrolling)
  float spiderHeight = H * 0.10f;
  // Matches Goal Island Y (0.15)
  lvl2.enemies.push_back(
      {{W * 0.5f, H * 0.15f - spiderHeight}, {0, 0}, true, 5, spider});

  // Sun: Top Right (Away from Exit)
  lvl2.sunPosition = {W - 100, 100.0f};

  lvl2.exitZone = {W * 0.5f - 50, H * 0.15f - 20, 100, 50};
}

void Game::LoadLevel(int index) {
  if (index >= levels.size())
    return;

  currentLevelIndex = index;
  Level &lvl = levels[currentLevelIndex];

  // Set Game State
  player.position = lvl.spawnPoint;
  player.velocity = {0, 0};
  player.hp = player.maxHp;
  player.isDead = false;

  // Spawn Enemies
  UnloadCurrentLevelEntities();
  for (const auto &config : lvl.enemies) {
    if (config.type == EnemyType::ROACH) {
      Roach *r = new Roach(config.position);
      r->sprite = roachTex;
      r->textureLoaded = true;
      r->width = (float)Core::SCREEN_HEIGHT * 0.25f;
      r->height = (float)Core::SCREEN_HEIGHT * 0.25f;
      currentEnemies.push_back(r);
    } else if (config.type == EnemyType::SPIDER) {
      Spider *s = new Spider(config.position);
      s->spritesheet = spiderSheet;
      s->frameCount = 2;
      s->frameSpeed = 0.2f;
      s->animated = true;
      s->width = (float)Core::SCREEN_HEIGHT * 0.10f;
      s->height = (float)Core::SCREEN_HEIGHT * 0.10f;
      currentEnemies.push_back(s);
    }
  }

  // Stop current music and start level music
  if (isMusicPlaying && currentPlayingMusic != nullptr) {
    StopMusicStream(*currentPlayingMusic);
    isMusicPlaying = false;
    currentPlayingMusic = nullptr;
  }

  // Start level music
  if (isDayTime && lvl.hasDayMusic) {
    PlayMusicStream(lvl.dayMusic);
    currentPlayingMusic = &lvl.dayMusic;
    isMusicPlaying = true;
  } else if (!isDayTime && lvl.hasNightMusic) {
    PlayMusicStream(lvl.nightMusic);
    currentPlayingMusic = &lvl.nightMusic;
    isMusicPlaying = true;
  }
}

void Game::ResetGame() { LoadLevel(currentLevelIndex); }

void Game::Update() {
  // Debug Toggle
  if (IsKeyPressed(KEY_H)) {
    debugMode = !debugMode;
  }

  // Update music stream
  if (isMusicPlaying && currentPlayingMusic != nullptr) {
    UpdateMusicStream(*currentPlayingMusic);
  }

  // State Machine
  switch (currentScreen) {
  case TITLE:
    // Update title music
    if (titleMusicLoaded) {
      UpdateMusicStream(titleMusic);
    }
    UpdateTitle();
    break;
  case STORY:
    if (titleMusicLoaded) {
      UpdateMusicStream(titleMusic);
    }
    UpdateStory();
    break;
  case GAMEPLAY:
    UpdateGameplay();
    break;
  case GAME_OVER:
    UpdateGameOver();
    break;
  case SETTINGS:
    UpdateSettings();
    break;
  case WIN:
    if (titleMusicLoaded) {
      UpdateMusicStream(titleMusic);
    }
    UpdateWin();
    break;
  }
}

void Game::UpdateTitle() {
  if (titleMusicLoaded && !IsMusicStreamPlaying(titleMusic)) {
    PlayMusicStream(titleMusic);
  }
  if (IsKeyPressed(KEY_ENTER)) {
    currentScreen = STORY;
  }
  if (IsKeyPressed(KEY_ESCAPE)) {
    previousScreen = TITLE;
    currentScreen = SETTINGS;
  }
}

void Game::UpdateStory() {
  if (IsKeyPressed(KEY_ENTER)) {
    // Stop title music
    if (titleMusicLoaded) {
      StopMusicStream(titleMusic);
    }
    currentScreen = GAMEPLAY;
    LoadLevel(0);
  }
}

void Game::UpdateGameOver() {
  if (IsKeyPressed(KEY_R)) {
    currentScreen = GAMEPLAY;
    ResetGame();
  }
}

void Game::UpdateGameplay() {
  float dt = GetFrameTime();

  // Level Switching (Test)
  if (IsKeyPressed(KEY_L)) {
    LoadLevel((currentLevelIndex + 1) % levels.size());
  }

  // Day/Night Toggle
  if (IsKeyPressed(KEY_T)) {
    isDayTime = !isDayTime;

    // Switch music on day/night toggle
    Level &lvl = levels[currentLevelIndex];
    if (isMusicPlaying && currentPlayingMusic != nullptr) {
      StopMusicStream(*currentPlayingMusic);
    }
    if (isDayTime && lvl.hasDayMusic) {
      PlayMusicStream(lvl.dayMusic);
      currentPlayingMusic = &lvl.dayMusic;
      isMusicPlaying = true;
    } else if (!isDayTime && lvl.hasNightMusic) {
      PlayMusicStream(lvl.nightMusic);
      currentPlayingMusic = &lvl.nightMusic;
      isMusicPlaying = true;
    }
  }

  // Update flower animation
  flowerAnimTimer += dt;
  if (flowerAnimTimer >= flowerAnimSpeed) {
    flowerAnimTimer -= flowerAnimSpeed;
    flowerAnimCurrentFrame =
        (flowerAnimCurrentFrame + 1) % flowerAnimFrameCount;
  }

  Level &currentLvl = levels[currentLevelIndex];

  // Dynamic Platform Logic (Flower)
  for (auto &plat : currentLvl.platforms) {
    if (plat.type == PlatformType::FLOWER) {
      float targetY = plat.initialY;
      if (!isDayTime) {
        targetY = plat.initialY + 200.0f;
      }

      float moveSpeed = 5.0f;
      float diff = targetY - plat.rect.y;
      float moveY = diff * moveSpeed * dt;
      plat.rect.y += moveY;

      Rectangle pRect = player.GetRect();
      if (pRect.x + pRect.width > plat.rect.x &&
          pRect.x < plat.rect.x + plat.rect.width) {
        float bottom = pRect.y + pRect.height;
        if (bottom <= plat.rect.y + 10 && bottom >= plat.rect.y - 10) {
          player.position.y += moveY;
        }
      }
    }
  }

  // Detect jump for sound
  bool wasGrounded = player.isGrounded;

  // Player Update
  player.Update(dt, currentLvl.platforms, isDayTime);

  // Play jump sound
  if (wasGrounded && !player.isGrounded && player.velocity.y < 0) {
    PlaySound(jumpSound);
  }

  // Enemies Update (only at night)
  if (!isDayTime) {
    for (auto *enemy : currentEnemies) {
      enemy->Update(dt, currentLvl);

      // Collision Check
      if (CheckCollisionRecs(player.GetRect(), enemy->GetRect())) {
        if (!player.isDead) {
          PlaySound(deathSound);
        }
        player.Die();
      }
    }
  }

  // Sun Damage (Raycast Logic)
  if (isDayTime) {
    bool isExposed = true;
    Rectangle playerRect = player.GetRect();
    Vector2 playerCenter = {playerRect.x + playerRect.width / 2.0f,
                            playerRect.y + playerRect.height / 2.0f};
    Vector2 sunPos = levels[currentLevelIndex].sunPosition;

    for (const auto &plat : currentLvl.platforms) {
      if (!plat.IsSolid(isDayTime))
        continue;
      if (CheckCollisionLineRect(sunPos, playerCenter, plat.rect)) {
        isExposed = false;
        break;
      }
    }

    if (isExposed) {
      player.TakeDamage(1.0f * dt);
      // Play burn sound occasionally (not every frame)
      if (!IsSoundPlaying(burnSound) && player.hp > 0) {
        PlaySound(burnSound);
      }
      // Show hint on first sun hit
      if (!sunHintShown) {
        sunHintShown = true;
        sunHintTimer = 4.0f; // Show for 4 seconds
      }
    }
  }

  // Game Over Check
  if (player.isDead) {
    if (isMusicPlaying && currentPlayingMusic != nullptr) {
      StopMusicStream(*currentPlayingMusic);
      isMusicPlaying = false;
      currentPlayingMusic = nullptr;
    }
    currentScreen = GAME_OVER;
  }

  // Check Exit Collision
  if (CheckCollisionRecs(player.GetRect(), currentLvl.exitZone)) {
    int nextLevel = currentLevelIndex + 1;
    if (nextLevel >= (int)levels.size()) {
      // All levels completed! Show win screen
      if (isMusicPlaying && currentPlayingMusic != nullptr) {
        StopMusicStream(*currentPlayingMusic);
        isMusicPlaying = false;
        currentPlayingMusic = nullptr;
      }
      if (titleMusicLoaded) {
        PlayMusicStream(titleMusic);
      }
      currentScreen = WIN;
    } else {
      LoadLevel(nextLevel);
    }
  }

  // ESC to settings
  if (IsKeyPressed(KEY_ESCAPE)) {
    previousScreen = GAMEPLAY;
    currentScreen = SETTINGS;
  }
}

void Game::Draw() {
  BeginDrawing();
  ClearBackground(BLACK);

  switch (currentScreen) {
  case TITLE:
    DrawTitle();
    break;
  case STORY:
    DrawStory();
    break;
  case GAMEPLAY:
    DrawGameplay();
    break;
  case SETTINGS:
    DrawSettings();
    break;
  case WIN:
    DrawWin();
    break;
  case GAME_OVER:
    DrawGameOver();
    break;
  }

  EndDrawing();
}

void Game::DrawTitle() {
  if (introScreenTex.id != 0) {
    // Draw intro screen with story - scaled to fill window
    Rectangle source = {0, 0, (float)introScreenTex.width,
                        (float)introScreenTex.height};
    Rectangle dest = {0, 0, (float)Core::SCREEN_WIDTH,
                      (float)Core::SCREEN_HEIGHT};
    DrawTexturePro(introScreenTex, source, dest, {0, 0}, 0.0f, WHITE);
  } else {
    ClearBackground(BLACK);
    DrawText("REINO DE ARAGON",
             Core::SCREEN_WIDTH / 2 - MeasureText("REINO DE ARAGON", 40) / 2,
             Core::SCREEN_HEIGHT / 3, 40, GOLD);
  }
  DrawText("PRESS ENTER TO START",
           Core::SCREEN_WIDTH / 2 - MeasureText("PRESS ENTER TO START", 20) / 2,
           Core::SCREEN_HEIGHT - 80, 20, WHITE);
}

void Game::DrawStory() {
  ClearBackground(BLACK);

  const char *storyText =
      "Dans un pot vit petit Jasmin.\n\n"
      "Petit Jasmin vit sa meilleur vie, arrose regulierement par un chill "
      "dude.\n"
      "Mais voila l'ete arrive, le soleil tape fort et la soif se fait "
      "ressentir.\n"
      "Depuis plusieurs jours, Petit Jasmin n'a pas ete arrose.\n"
      "Il semblerait que plus personne ne soit la pour s'en occuper.\n\n"
      "Il va falloir sortir de son pot et chercher de l'eau,\n"
      "sous peine de mourir dans d'atroce souffrance,\n"
      "petit a petit sous la chaleur ecrasante du soleil cuisant,\n"
      "terrible destin fatale.\n\n"
      "Mais Petit Jasmin n'est jamais sortie de son pot\n"
      "(parce que les plantes sa sort pas des pots),\n"
      "mais il va falloir defier toute logique pour chercher de l'eau,\n"
      "mais attention, hors de son pot,\n"
      "Petit Jasmin est encore plus sensible au soleil !";

  // Draw story text
  int fontSize = 20;
  int startY = 80;
  int x = 80;

  // Title
  DrawText("L'HISTOIRE DE PETIT JASMIN",
           Core::SCREEN_WIDTH / 2 -
               MeasureText("L'HISTOIRE DE PETIT JASMIN", 32) / 2,
           25, 32, GOLD);

  // Draw text
  DrawText(storyText, x, startY, fontSize, WHITE);

  // Draw intro image if available (bottom right)
  if (introImageTex.id != 0) {
    float imgScale = 0.6f;
    float drawW = introImageTex.width * imgScale;
    float drawH = introImageTex.height * imgScale;
    float drawX = Core::SCREEN_WIDTH - drawW - 30;
    float drawY = Core::SCREEN_HEIGHT - drawH - 60;
    Rectangle src = {0, 0, (float)introImageTex.width,
                     (float)introImageTex.height};
    Rectangle dst = {drawX, drawY, drawW, drawH};
    DrawTexturePro(introImageTex, src, dst, {0, 0}, 0.0f, WHITE);
  }

  DrawText("PRESS ENTER TO PLAY",
           Core::SCREEN_WIDTH / 2 - MeasureText("PRESS ENTER TO PLAY", 20) / 2,
           Core::SCREEN_HEIGHT - 40, 20, GOLD);
}

void Game::DrawGameOver() {
  if (gameOverScreenTex.id != 0) {
    Rectangle source = {0, 0, (float)gameOverScreenTex.width,
                        (float)gameOverScreenTex.height};
    Rectangle dest = {0, 0, (float)Core::SCREEN_WIDTH,
                      (float)Core::SCREEN_HEIGHT};
    DrawTexturePro(gameOverScreenTex, source, dest, {0, 0}, 0.0f, WHITE);
  } else {
    DrawRectangle(0, 0, Core::SCREEN_WIDTH, Core::SCREEN_HEIGHT, BLACK);
    DrawText("GAME OVER",
             Core::SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 60) / 2,
             Core::SCREEN_HEIGHT / 3, 60, RED);
  }
  DrawText("PRESS 'R' TO RESTART",
           Core::SCREEN_WIDTH / 2 - MeasureText("PRESS 'R' TO RESTART", 30) / 2,
           Core::SCREEN_HEIGHT / 2, 30, WHITE);
}

// Ray Intersection Helper
Vector2 GetRayIntersection(Vector2 start, Vector2 end,
                           const std::vector<Platform> &platforms, bool isDay) {
  float minT = 1.0f;

  for (const auto &plat : platforms) {
    if (!plat.IsSolid(isDay))
      continue;

    if (CheckCollisionLineRect(start, end, plat.rect)) {
      float t0 = 0.0f;
      float t1 = minT;
      for (int i = 0; i < 8; i++) {
        float mid = t0 + (t1 - t0) * 0.5f;
        Vector2 midPoint = {start.x + (end.x - start.x) * mid,
                            start.y + (end.y - start.y) * mid};
        if (CheckCollisionPointRec(midPoint, plat.rect)) {
          t1 = mid;
        } else {
          if (CheckCollisionLineRect(start, midPoint, plat.rect)) {
            t1 = mid;
          } else {
            t0 = mid;
          }
        }
      }
      if (t1 < minT)
        minT = t1;
    }
  }

  return {start.x + (end.x - start.x) * minT,
          start.y + (end.y - start.y) * minT};
}

void Game::DrawGameplay() {
  // Draw Background
  Level &currentLvl = levels[currentLevelIndex];
  Texture2D bg = isDayTime ? std::get<0>(currentLvl.backgrounds)
                           : std::get<1>(currentLvl.backgrounds);
  // Scale background to fill screen
  Rectangle bgSource = {0, 0, (float)bg.width, (float)bg.height};
  Rectangle bgDest = {0, 0, (float)Core::SCREEN_WIDTH,
                      (float)Core::SCREEN_HEIGHT};
  DrawTexturePro(bg, bgSource, bgDest, {0, 0}, 0.0f, WHITE);

  // Draw Sun/Moon
  if (isDayTime) {
    DrawCircleV(currentLvl.sunPosition, 60, YELLOW);
    DrawCircleV(currentLvl.sunPosition, 70, Fade(GOLD, 0.3f));

    // --- GOD RAYS VISUALS ---
    Vector2 sunPos = currentLvl.sunPosition;
    Color rayColor = Fade(YELLOW, 0.15f);

    int screenW = Core::SCREEN_WIDTH;
    int screenH = Core::SCREEN_HEIGHT;
    int step = 40;

    for (int x = 0; x <= screenW; x += step) {
      Vector2 target = {(float)x, (float)screenH};
      Vector2 endPoint =
          GetRayIntersection(sunPos, target, currentLvl.platforms, isDayTime);
      DrawLineV(sunPos, endPoint, rayColor);
    }
  }

  // Draw Platforms
  for (const auto &plat : currentLvl.platforms) {
    if (plat.type == PlatformType::INVISIBLE && !debugMode)
      continue;

    if (plat.type == PlatformType::FLOWER) {
      // Draw animated flower platform
      if (flowerAnimSheet.id != 0) {
        float frameW =
            (float)flowerAnimSheet.width / (float)flowerAnimFrameCount;
        float frameH = (float)flowerAnimSheet.height;
        Rectangle source = {frameW * flowerAnimCurrentFrame, 0, frameW, frameH};
        float drawHeight = plat.rect.height * 3.0f;
        Rectangle dest = {plat.rect.x - plat.rect.width * 0.25f,
                          plat.rect.y - drawHeight + plat.rect.height,
                          plat.rect.width * 1.5f, drawHeight};

        float alpha = plat.IsSolid(isDayTime) ? 1.0f : 0.3f;
        DrawTexturePro(flowerAnimSheet, source, dest, {0, 0}, 0.0f,
                       Fade(WHITE, alpha));
      } else {
        Color c = plat.color;
        if (!plat.IsSolid(isDayTime) && !debugMode)
          c = Fade(c, 0.3f);
        DrawRectangleRec(plat.rect, c);
      }
    } else if (plat.type == PlatformType::MUSHROOM) {
      Texture2D mushTex = isDayTime ? mushroomDayTex : mushroomNightTex;
      if (mushTex.id != 0) {
        Rectangle source = {0, 0, (float)mushTex.width, (float)mushTex.height};
        Rectangle dest = {plat.rect.x, plat.rect.y, plat.rect.width,
                          plat.rect.height};
        float alpha = plat.IsSolid(isDayTime) ? 1.0f : 0.3f;
        DrawTexturePro(mushTex, source, dest, {0, 0}, 0.0f, Fade(WHITE, alpha));
      } else {
        Color c = plat.color;
        if (!plat.IsSolid(isDayTime) && !debugMode)
          c = Fade(c, 0.3f);
        DrawRectangleRec(plat.rect, c);
      }
    } else if (plat.type == PlatformType::NORMAL) {
      // Use cropped platform textures (434x457 after auto-crop)
      Texture2D platTex = isDayTime ? platformDayTex : platformNightTex;
      if (platTex.id != 0 && plat.rect.width > 0 && plat.rect.height > 0) {
        Rectangle source = {0, 0, (float)platTex.width, (float)platTex.height};
        Rectangle dest = {plat.rect.x, plat.rect.y, plat.rect.width,
                          plat.rect.height};
        DrawTexturePro(platTex, source, dest, {0, 0}, 0.0f, WHITE);
      } else {
        Color platColor =
            isDayTime ? Color{101, 67, 33, 255} : Color{50, 35, 20, 255};
        DrawRectangleRec(plat.rect, platColor);
      }
    } else {
      // Invisible or other
      if (debugMode) {
        Color c = Fade(LIME, 0.3f);
        DrawRectangleRec(plat.rect, c);
      }
    }

    if (debugMode) {
      DrawRectangleLinesEx(plat.rect, 2, RED);
    }
  }

  // Draw Foreground Layer (BEFORE entities so player is visible on top)
  if (currentLvl.hasForeground) {
    Texture2D fg =
        isDayTime ? currentLvl.foregroundDay : currentLvl.foregroundNight;
    if (fg.id != 0) {
      Rectangle fgSource = {0, 0, (float)fg.width, (float)fg.height};
      Rectangle fgDest = {0, 0, (float)Core::SCREEN_WIDTH,
                          (float)Core::SCREEN_HEIGHT};
      DrawTexturePro(fg, fgSource, fgDest, {0, 0}, 0.0f, Fade(WHITE, 0.5f));
    }
  }

  // Draw Exit Zone with watering can image
  {
    Texture2D wpTex = isDayTime ? waterPotDayTex : waterPotNightTex;
    if (wpTex.id != 0) {
      Rectangle source = {0, 0, (float)wpTex.width, (float)wpTex.height};
      // Draw proportionally - images are now cropped (700x643)
      float aspect = (float)wpTex.width / (float)wpTex.height;
      float drawH = currentLvl.exitZone.height * 1.8f;
      float drawW = drawH * aspect;
      float drawX = currentLvl.exitZone.x + currentLvl.exitZone.width / 2.0f -
                    drawW / 2.0f;
      float drawY = currentLvl.exitZone.y + currentLvl.exitZone.height - drawH;
      Rectangle dest = {drawX, drawY, drawW, drawH};
      DrawTexturePro(wpTex, source, dest, {0, 0}, 0.0f, WHITE);
    } else {
      DrawRectangleRec(currentLvl.exitZone, GOLD);
      DrawText("EXIT", (int)currentLvl.exitZone.x + 10,
               (int)currentLvl.exitZone.y + 10, 20, WHITE);
    }
  }

  // Draw Entities (ON TOP of foreground so player is visible)
  player.Draw();
  if (debugMode)
    DrawRectangleLinesEx(player.GetRect(), 2, GREEN);

  // Draw enemies (only at night)
  if (!isDayTime) {
    for (auto *enemy : currentEnemies) {
      enemy->Draw();
      if (debugMode)
        DrawRectangleLinesEx(enemy->GetRect(), 2, RED);
    }
  }

  // UI
  DrawText(isDayTime ? "DAY" : "NIGHT", 20, 20, 20, isDayTime ? BLACK : WHITE);
  char levelBuf[32];
  snprintf(levelBuf, sizeof(levelBuf), "LEVEL %d", currentLevelIndex + 1);
  DrawText(levelBuf, 100, 20, 20, RED);

  if (debugMode)
    DrawText("DEBUG MODE ON", 20, 50, 20, RED);

  // Sun hint popup
  if (sunHintTimer > 0.0f) {
    sunHintTimer -= GetFrameTime();
    float alpha =
        sunHintTimer > 1.0f ? 1.0f : sunHintTimer; // Fade out last second
    const char *hint = "Appuie sur T pour passer en mode Nuit !";
    int hintFontSize = 24;
    int textW = MeasureText(hint, hintFontSize);
    int hintX = Core::SCREEN_WIDTH / 2 - textW / 2;
    int hintY = Core::SCREEN_HEIGHT / 2 - 50;
    // Dark background box
    DrawRectangle(hintX - 15, hintY - 10, textW + 30, 44,
                  Fade(BLACK, 0.7f * alpha));
    DrawText(hint, hintX, hintY, hintFontSize, Fade(YELLOW, alpha));
  }

  // HEALTHBAR
  float hpPct = player.hp / player.maxHp;
  float barWidth = 200.0f;
  float barHeight = 20.0f;
  Vector2 barPos = {(float)Core::SCREEN_WIDTH - barWidth - 20, 20};

  DrawRectangleV(barPos, {barWidth, barHeight}, DARKGRAY);
  DrawRectangleV(barPos, {barWidth * hpPct, barHeight}, RED);
  DrawRectangleLines(barPos.x, barPos.y, barWidth, barHeight, WHITE);
  DrawText("HP", barPos.x - 30, barPos.y, 20, RED);

  if (isDayTime) {
    DrawText("SUN HURTS!", barPos.x, barPos.y + 30, 20, ORANGE);
  }
}

void Game::Unload() {
  if (isUnloaded)
    return;
  isUnloaded = true;

  UnloadCurrentLevelEntities();

  // Unload all textures
  UnloadTexture(playerIdleTex);
  UnloadTexture(playerWalkSheet);
  UnloadTexture(playerDeathTex);
  UnloadTexture(roachTex);
  UnloadTexture(spiderSheet);
  UnloadTexture(introScreenTex);
  UnloadTexture(introImageTex);
  UnloadTexture(gameOverScreenTex);
  UnloadTexture(waterPotDayTex);
  UnloadTexture(waterPotNightTex);
  UnloadTexture(flowerTex);
  UnloadTexture(flowerAnimSheet);
  UnloadTexture(mushroomDayTex);
  UnloadTexture(mushroomNightTex);
  UnloadTexture(platformDayTex);
  UnloadTexture(platformNightTex);

  // Unload sounds
  UnloadSound(jumpSound);
  UnloadSound(walkSound);
  UnloadSound(burnSound);
  UnloadSound(deathSound);
  UnloadSound(wateringCanSound);

  // Unload title music
  if (titleMusicLoaded) {
    UnloadMusicStream(titleMusic);
  }

  // Levels unload their own resources
  for (auto &lvl : levels) {
    lvl.Unload();
  }
  levels.clear();
}

// --- Settings Menu ---

void Game::ApplyVolume() {
  SetMasterVolume(masterVolume);
  // Apply music volume to currently playing music
  if (isMusicPlaying && currentPlayingMusic != nullptr) {
    SetMusicVolume(*currentPlayingMusic, musicVolume);
  }
  if (titleMusicLoaded && IsMusicStreamPlaying(titleMusic)) {
    SetMusicVolume(titleMusic, musicVolume);
  }
  // SFX volume applied per-sound when played
  SetSoundVolume(jumpSound, sfxVolume);
  SetSoundVolume(walkSound, sfxVolume);
  SetSoundVolume(burnSound, sfxVolume);
  SetSoundVolume(deathSound, sfxVolume);
  SetSoundVolume(wateringCanSound, sfxVolume);
}

void Game::ApplyResolution() {
  int newW = resOptions[selectedResIndex].width;
  int newH = resOptions[selectedResIndex].height;
  Core::SCREEN_WIDTH = newW;
  Core::SCREEN_HEIGHT = newH;
  Core::RecalculatePhysics();
  SetWindowSize(newW, newH);

  // Rebuild level geometry for new resolution
  RebuildLevelGeometry();

  // Resize player
  player.width = (float)Core::SCREEN_HEIGHT * 0.12f;
  player.height = (float)Core::SCREEN_HEIGHT * 0.12f;

  // Reload current level if in gameplay
  LoadLevel(currentLevelIndex);
}

// RebuildLevelGeometry moved to after Init()

void Game::UpdateSettings() {
  // Navigate with UP/DOWN
  if (IsKeyPressed(KEY_UP)) {
    settingsSelection--;
    if (settingsSelection < 0)
      settingsSelection = 4;
  }
  if (IsKeyPressed(KEY_DOWN)) {
    settingsSelection++;
    if (settingsSelection > 4)
      settingsSelection = 0;
  }

  // Adjust with LEFT/RIGHT
  if (settingsSelection == 0) {
    // Resolution
    if (IsKeyPressed(KEY_LEFT)) {
      selectedResIndex--;
      if (selectedResIndex < 0)
        selectedResIndex = RES_COUNT - 1;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
      selectedResIndex++;
      if (selectedResIndex >= RES_COUNT)
        selectedResIndex = 0;
    }
    if (IsKeyPressed(KEY_ENTER)) {
      ApplyResolution();
    }
  } else if (settingsSelection == 1) {
    // Music Volume
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
      musicVolume -= 0.1f;
      if (musicVolume < 0.0f)
        musicVolume = 0.0f;
      ApplyVolume();
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
      musicVolume += 0.1f;
      if (musicVolume > 1.0f)
        musicVolume = 1.0f;
      ApplyVolume();
    }
  } else if (settingsSelection == 2) {
    // SFX Volume
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
      sfxVolume -= 0.1f;
      if (sfxVolume < 0.0f)
        sfxVolume = 0.0f;
      ApplyVolume();
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
      sfxVolume += 0.1f;
      if (sfxVolume > 1.0f)
        sfxVolume = 1.0f;
      ApplyVolume();
    }
  } else if (settingsSelection == 3) {
    // Fullscreen
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_LEFT) ||
        IsKeyPressed(KEY_RIGHT)) {
      isFullscreen = !isFullscreen;
      ToggleFullscreen();
    }
  } else if (settingsSelection == 4) {
    // Back
    if (IsKeyPressed(KEY_ENTER)) {
      currentScreen = previousScreen;
    }
  }

  // ESC to go back
  if (IsKeyPressed(KEY_ESCAPE)) {
    currentScreen = previousScreen;
  }
}

void Game::DrawSettings() {
  // Draw a semi-transparent overlay if coming from gameplay
  if (previousScreen == GAMEPLAY) {
    DrawGameplay();
    DrawRectangle(0, 0, Core::SCREEN_WIDTH, Core::SCREEN_HEIGHT,
                  Fade(BLACK, 0.75f));
  } else {
    ClearBackground(Color{20, 20, 30, 255});
  }

  int centerX = Core::SCREEN_WIDTH / 2;
  int startY = Core::SCREEN_HEIGHT / 4;
  int spacing = 50;
  int fontSize = 24;

  // Title
  DrawText("SETTINGS", centerX - MeasureText("SETTINGS", 36) / 2, startY - 60,
           36, GOLD);

  // Menu items
  const char *labels[] = {"Resolution", "Music Volume", "SFX Volume",
                          "Fullscreen", "Back"};

  for (int i = 0; i < 5; i++) {
    Color textColor = (i == settingsSelection) ? GOLD : LIGHTGRAY;
    int y = startY + i * spacing;

    // Highlight arrow
    if (i == settingsSelection) {
      DrawText(">", centerX - 200, y, fontSize, GOLD);
    }

    DrawText(labels[i], centerX - 170, y, fontSize, textColor);

    // Values
    if (i == 0) {
      // Resolution
      const char *resLabel = resOptions[selectedResIndex].label;
      char buf[64];
      snprintf(buf, sizeof(buf), "< %s >", resLabel);
      DrawText(buf, centerX + 80, y, fontSize, textColor);
      if (i == settingsSelection) {
        DrawText("(Press ENTER to apply)", centerX + 80, y + 25, 14, DARKGRAY);
      }
    } else if (i == 1) {
      // Music Volume bar
      float barW = 150;
      float barH = 20;
      float barX = centerX + 80;
      float barY = y + 3;
      DrawRectangle(barX, barY, barW, barH, DARKGRAY);
      DrawRectangle(barX, barY, barW * musicVolume, barH, GREEN);
      DrawRectangleLines(barX, barY, barW, barH, WHITE);
      char vol[16];
      snprintf(vol, sizeof(vol), "%d%%", (int)(musicVolume * 100));
      DrawText(vol, barX + barW + 10, y, fontSize, textColor);
    } else if (i == 2) {
      // SFX Volume bar
      float barW = 150;
      float barH = 20;
      float barX = centerX + 80;
      float barY = y + 3;
      DrawRectangle(barX, barY, barW, barH, DARKGRAY);
      DrawRectangle(barX, barY, barW * sfxVolume, barH, BLUE);
      DrawRectangleLines(barX, barY, barW, barH, WHITE);
      char vol[16];
      snprintf(vol, sizeof(vol), "%d%%", (int)(sfxVolume * 100));
      DrawText(vol, barX + barW + 10, y, fontSize, textColor);
    } else if (i == 3) {
      // Fullscreen toggle
      DrawText(isFullscreen ? "ON" : "OFF", centerX + 80, y, fontSize,
               isFullscreen ? GREEN : RED);
    }
  }

  DrawText(
      "UP/DOWN: Navigate  |  LEFT/RIGHT: Adjust  |  ESC: Back",
      centerX -
          MeasureText("UP/DOWN: Navigate  |  LEFT/RIGHT: Adjust  |  ESC: Back",
                      14) /
              2,
      Core::SCREEN_HEIGHT - 40, 14, GRAY);
}

// --- Win Screen ---

void Game::UpdateWin() {
  if (IsKeyPressed(KEY_ENTER)) {
    currentScreen = TITLE;
  }
}

void Game::DrawWin() {
  ClearBackground(Color{10, 30, 10, 255});

  int centerX = Core::SCREEN_WIDTH / 2;

  DrawText("FELICITATIONS !", centerX - MeasureText("FELICITATIONS !", 48) / 2,
           Core::SCREEN_HEIGHT / 4, 48, GOLD);

  DrawText("Petit Jasmin a trouve de l'eau !",
           centerX - MeasureText("Petit Jasmin a trouve de l'eau !", 24) / 2,
           Core::SCREEN_HEIGHT / 4 + 80, 24, WHITE);

  DrawText("Merci d'avoir joue a Reino de Aragon",
           centerX -
               MeasureText("Merci d'avoir joue a Reino de Aragon", 20) / 2,
           Core::SCREEN_HEIGHT / 2, 20, LIGHTGRAY);

  // Draw intro image if available
  if (introImageTex.id != 0) {
    float scale = 0.5f;
    float drawW = introImageTex.width * scale;
    float drawH = introImageTex.height * scale;
    Rectangle src = {0, 0, (float)introImageTex.width,
                     (float)introImageTex.height};
    Rectangle dst = {centerX - drawW / 2, (float)Core::SCREEN_HEIGHT / 2 + 40,
                     drawW, drawH};
    DrawTexturePro(introImageTex, src, dst, {0, 0}, 0.0f, WHITE);
  }

  DrawText("PRESS ENTER TO RETURN TO MENU",
           centerX - MeasureText("PRESS ENTER TO RETURN TO MENU", 20) / 2,
           Core::SCREEN_HEIGHT - 60, 20, GOLD);
}
