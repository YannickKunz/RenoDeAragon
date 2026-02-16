#include "level.h"
#include "constants.h"
#include <tuple>

// --- Helper Methods Implementation ---
void Level::AddCloud(float x, float y, float width, float height, float speed,
                     float leftLimit, float rightLimit, bool movingRight) {
  clouds.push_back(
      {{x, y, width, height}, speed, leftLimit, rightLimit, movingRight});
}

void Level::AddPlatform(float x, float y, float width, float height,
                        PlatformType type) {
  platforms.push_back({{x, y, width, height}, type});
}

void Level::AddEnemy(float x, float y, float width, float height,
                     bool patrolSide, int patrolPlatformIndex, EnemyType type) {
  enemies.push_back(
      {{x, y}, {width, height}, patrolSide, patrolPlatformIndex, type});
}

void Level::Unload() {
  UnloadTexture(std::get<0>(backgrounds));
  UnloadTexture(std::get<1>(backgrounds));
  UnloadTexture(texBasic);
  UnloadTexture(texMushroom);
  UnloadTexture(texFlower);
}

static Texture2D dummyLoad(const char *f) { return LoadTexture(f); }

// Helper to auto-crop transparent borders
Texture2D LoadAndCrop(const char *fileName) {
  Image img = LoadImage(fileName);
  if (img.data == NULL) {
    std::cout << "ERROR: Failed to load image " << fileName << std::endl;
    return {0};
  }

  // Find the bounding box of non-transparent pixels
  // Increase threshold to 0.05 to ignore slight noise
  Rectangle crop = GetImageAlphaBorder(img, 0.05f);

  std::cout << "AutoCrop " << fileName << ": Original " << img.width << "x"
            << img.height << " -> Crop " << crop.x << "," << crop.y << " "
            << crop.width << "x" << crop.height << std::endl;

  // If the image is empty or full transparent, crop might be weird.
  // Check validity.
  if (crop.width > 0 && crop.height > 0) {
    ImageCrop(&img, crop);
  }

  Texture2D tex = LoadTextureFromImage(img);
  UnloadImage(img);
  return tex;
}

std::vector<Level> InitLevels() {
  std::vector<Level> levels;

  // LEVEL 1
  Level lvl1;

  Image dayImg = LoadImage("images:anims/level1day.png");
  Image nightImg = LoadImage("images:anims/level1night.jpg");

  ImageResize(&dayImg, SCREEN_WIDTH, SCREEN_HEIGHT);
  ImageResize(&nightImg, SCREEN_WIDTH, SCREEN_HEIGHT);

  Texture2D dayTex = LoadTextureFromImage(dayImg);
  Texture2D nightTex = LoadTextureFromImage(nightImg);

  UnloadImage(dayImg);
  UnloadImage(nightImg);

  // Load Per-Level Textures
  // Mushrooms and Flowers are sprite sheets (2 frames), AutoCrop breaks them.
  // Revert to LoadTexture and manual pruning in drawPlatform.
  lvl1.texBasic = LoadTexture("images:anims/PlatformTextureLevel1.png");
  lvl1.texMushroom = LoadTexture("images:anims/mushroomDayUpDown.png");
  lvl1.texFlower = LoadTexture("images:anims/flower-1.png");

  lvl1.backgrounds = std::make_tuple(dayTex, nightTex);
  lvl1.spawnPoint = {SCREEN_WIDTH - 50, (float)(SCREEN_HEIGHT - 100)};

  // Using new helper methods for clarity
  lvl1.AddPlatform(SCREEN_WIDTH - 550, SCREEN_HEIGHT / 2.0f, 550, 30, basic);
  lvl1.AddPlatform(0, SCREEN_HEIGHT / 2.0f, 300, 30, basic);
  lvl1.AddPlatform(400, SCREEN_HEIGHT - 125, 100, 100, mushroom);
  lvl1.AddPlatform(0, (float)(SCREEN_HEIGHT - 70), (float)SCREEN_WIDTH, 50,
                   invisible); // Ground

  lvl1.sunPosition = {SCREEN_WIDTH - 100, -50.0f};
  lvl1.isDay = true;
  lvl1.exitZone = {(float)SCREEN_WIDTH - 100, 150, 50, 100};
  lvl1.musicPath = {"music:sounds/lvlupjam_lvl1.wav",
                    "music:sounds/lvlupjam_lvl1_night.wav"};
  levels.push_back(lvl1);

  // LEVEL 2
  Level lvl2;

  Image dayImg2 = LoadImage("images:anims/level2day.png");
  Image nightImg2 = LoadImage("images:anims/level2night.png");

  ImageResize(&dayImg2, SCREEN_WIDTH, SCREEN_HEIGHT);
  ImageResize(&nightImg2, SCREEN_WIDTH, SCREEN_HEIGHT);

  Texture2D dayTex2 = LoadTextureFromImage(dayImg2);
  Texture2D nightTex2 = LoadTextureFromImage(nightImg2);

  UnloadImage(dayImg2);
  UnloadImage(nightImg2);

  lvl2.texBasic = LoadTexture("images:anims/PlatformTextureLevel1Night.png");
  lvl2.texMushroom = LoadTexture("images:anims/mushroomDayUpDown.png");
  lvl2.texFlower = LoadTexture("images:anims/flower-1.png");

  lvl2.backgrounds = std::make_tuple(dayTex2, nightTex2);
  lvl2.spawnPoint = {50, (float)(SCREEN_HEIGHT - 100)};

  // Platforms
  lvl2.AddPlatform(SCREEN_WIDTH - 250, (SCREEN_HEIGHT - 150), 100, 100, flower);
  lvl2.AddPlatform(0, 250, 200, 50, basic);
  lvl2.AddPlatform(300, 350, 200, 50, basic);
  lvl2.AddPlatform(600, 450, 200, 50, basic);
  lvl2.AddPlatform(600, (float)(SCREEN_HEIGHT - 50), 10, 100,
                   invisible); // fake platform for mob
  lvl2.AddPlatform(50, -200, 375, 50, basic);
  lvl2.AddPlatform(SCREEN_WIDTH - SCREEN_WIDTH / 3, -200, 375, 50, basic);
  lvl2.AddPlatform(0, (float)(SCREEN_HEIGHT - 70), (float)SCREEN_WIDTH, 50,
                   invisible); // Ground

  int fake_platform_idx = 4; // Index of the invisible platform above

  // Enemies
  // Uses platform position for spawn logic
  float enemyX = lvl2.platforms[fake_platform_idx].position.x;
  float enemyY = lvl2.platforms[fake_platform_idx].position.y;
  lvl2.AddEnemy(enemyX, enemyY, 100, 100, true, fake_platform_idx, roach);

  lvl2.sunPosition = {800.0f, -1200.0f};
  lvl2.exitZone = {25, 200, 100, 50};
  lvl2.isDay = false;
  lvl2.musicPath = {"music:sounds/lvlupjam_lvl2.wav",
                    "music:sounds/lvlupjam_lvl2_night.wav"};
  levels.push_back(lvl2);

  // LEVEL 3
  Level lvl3;

  Image dayImg3 = LoadImage("images:anims/level3day.png");
  Image nightImg3 = LoadImage("images:anims/level3night.png");

  ImageResize(&dayImg3, SCREEN_WIDTH, SCREEN_HEIGHT);
  ImageResize(&nightImg3, SCREEN_WIDTH, SCREEN_HEIGHT);

  Texture2D dayTex3 = LoadTextureFromImage(dayImg3);
  Texture2D nightTex3 = LoadTextureFromImage(nightImg3);

  UnloadImage(dayImg3);
  UnloadImage(nightImg3);

  lvl3.texBasic = LoadTexture("images:anims/PlatformTextureLevel1.png");
  lvl3.texMushroom = LoadTexture("images:anims/nightShroom.png");
  lvl3.texFlower = LoadTexture("images:anims/flower-1.png");

  lvl3.backgrounds = std::make_tuple(dayTex3, nightTex3);
  lvl3.spawnPoint = {100, 600};

  // Platforms
  lvl3.AddPlatform(0, 400, 450, 50, basic);
  lvl3.AddPlatform(600, 600, 100, 100, flower);
  lvl3.AddPlatform(800, 450, 200, 50, basic); // Index 2
  lvl3.AddPlatform(1000, 300, 200, 50, basic);
  lvl3.AddPlatform(0, (float)(SCREEN_HEIGHT - 70), (float)SCREEN_WIDTH, 50,
                   invisible); // Ground

  // Enemies
  // Spawns on platform index 2
  float enemy3X =
      lvl3.platforms[2].position.x + lvl3.platforms[2].position.width / 2;
  float enemy3Y = lvl3.platforms[2].position.y;
  lvl3.AddEnemy(
      enemy3X, enemy3Y, 100, 100, false, 2,
      spider); // Assuming spider for this one based on context, or default

  // Clouds
  // x, y, width, height, speed, leftLimit, rightLimit, movingRight
  lvl3.AddCloud(100, -50, 150, 40, 75.0f, 200, 800, true);
  lvl3.AddCloud(SCREEN_WIDTH, -50, 150, 40, 75.0f, -100, 900, false);

  lvl3.sunPosition = {SCREEN_WIDTH - 50, -1000.0f};
  lvl3.isDay = true;
  lvl3.musicPath = {"music:sounds/lvlupjam_lvl3.wav",
                    "music:sounds/lvlupjam_lvl3.wav"};
  lvl3.exitZone = {25, 200, 100, 50};
  levels.push_back(lvl3);

  return levels;
}
