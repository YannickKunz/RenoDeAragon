#include "player.h"

#define TOGGLE_DELAY_SEC 2.0f
#define G 800
#define JUMP_SPEED 600.0f
#define MOVEMENT 200.0f
#define MUSHROOM_COEFF 1.2f

void updatePlayer(Player &player, std::vector<Platform> &platforms,
                  std::vector<Enemy> &enemies, const float delta,
                  Vector2 spawnPoint, bool &playerToggle, Sound &sndJump,
                  Sound &sndWalk) {
  player.toggleCooldown += delta; // could this overflow?

  bool isMoving = false;
  if (IsKeyDown(KEY_A)) {
    player.position.x -= MOVEMENT * delta;
    player.isFacingRight = false;
    isMoving = true;
  }
  if (IsKeyDown(KEY_D)) {
    player.position.x += MOVEMENT * delta;
    player.isFacingRight = true;
    isMoving = true;
  }
  if (IsKeyPressed(KEY_F) && (player.toggleCooldown >= TOGGLE_DELAY_SEC)) {
    playerToggle = !playerToggle;
    player.toggleCooldown = 0.0f;
    player.frameRec = {0.0f, 0.0f, (float)player.texture.width / 6,
                       (float)player.texture.height};
  }
  if (IsKeyDown(KEY_SPACE) && player.canJump) {
    player.speed -= JUMP_SPEED;
    player.canJump = false;
    PlaySound(sndJump);
  }
  // --- ANIMATION INITIALIZATION (SAFETY CHECK) ---
  // If variables are 0, we can't see the player. Initialize them here.
  if (player.framesSpeed == 0)
    player.framesSpeed = 8;
  if (player.frameRec.width == 0 && player.texture.id > 0) {
    player.frameRec = {0.0f, 0.0f, (float)player.texture.width / 6,
                       (float)player.texture.height};
  }

  // ANIMATION LOGIC
  // ANIMATION LOGIC (Only runs if moving)
  if (isMoving) {
    player.framesCounter++;

    // Switch frames every (60 / framesSpeed) calls
    if (player.framesCounter >= (60 / player.framesSpeed)) {
      player.framesCounter = 0;
      player.currentFrame++;

      // Play walk sound on specific frames or just when moving?
      // Let's play it if not playing to loop it, or just trigger it
      if (!IsSoundPlaying(sndWalk))
        PlaySound(sndWalk);

      // Reset if we exceed the number of frames in the sheet (e.g., 6)
      if (player.currentFrame > 5)
        player.currentFrame = 0;

      // Move the rectangle window to the next frame
      player.frameRec.x =
          (float)player.currentFrame * (float)player.texture.width / 6;
    }
  } else {
    // Reset to IDLE frame (0) when standing still
    player.framesCounter = 0;
    player.currentFrame = 0;
    player.frameRec.x = 0;
  }

  // FLIP LOGIC
  // Use isFacingRight instead of checking keys again
  if (!player.isFacingRight)
    player.frameRec.width = -fabs(player.frameRec.width);
  else
    player.frameRec.width = fabs(player.frameRec.width);

  // Check horizontal collisions (player sides vs platform sides)
  for (int i = 0; i < platforms.size(); i++) {
    Rectangle platformPosition = platforms[i].position;
    if (!isPlatformActive(platforms[i], playerToggle)) {
      continue;
    }
    float playerLeft = player.position.x - player.size.x / 2;
    float playerRight = player.position.x + player.size.x / 2;
    float playerTop = player.position.y - player.size.y;
    float playerBottom = player.position.y;

    // Check if vertically overlapping
    if (playerBottom > platformPosition.y &&
        playerTop < platformPosition.y + platformPosition.height) {
      // Check if horizontally overlapping
      if (playerRight > platformPosition.x &&
          playerLeft < platformPosition.x + platformPosition.width) {
        // Push out from whichever side is closer
        float overlapLeft = playerRight - platformPosition.x;
        float overlapRight =
            (platformPosition.x + platformPosition.width) - playerLeft;
        if (overlapLeft < overlapRight) {
          player.position.x =
              platformPosition.x - player.size.x / 2; // push left
        } else {
          player.position.x = platformPosition.x + platformPosition.width +
                              player.size.x / 2; // push right
        }
      }
    }
  }
  // Collision with enemy
  Rectangle playerRect = {player.position.x - player.size.x / 2,
                          player.position.y - player.size.y, player.size.x,
                          player.size.y};

  if (isEnemyActive(playerToggle)) {
    for (Enemy &enemy : enemies) {
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
    }
  }

  // --- Vertical collision ---
  bool hitObstacle = false;
  for (int i = 0; i < platforms.size(); i++) {
    Rectangle platformPosition = platforms[i].position;
    if (!isPlatformActive(platforms[i], playerToggle)) {
      continue;
    }

    float playerLeft = player.position.x - player.size.x / 2;
    float playerRight = player.position.x + player.size.x / 2;
    if (playerRight >= platformPosition.x &&
        playerLeft <= (platformPosition.x + platformPosition.width)) {
      // Landing on top of platform (falling down)
      if (player.speed > 0 && platformPosition.y >= player.position.y &&
          platformPosition.y <= (player.position.y + player.speed * delta)) {
        if (platforms[i].type == mushroom) {
          // jump
          player.speed = -MUSHROOM_COEFF * JUMP_SPEED;
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
}
void drawPlayer(Player &player) {
  // Calculate visual dimensions based on the sprite frame

  // Manual Trim based on logs: Content is ~620px tall starting at y=460
  Rectangle source = player.frameRec;
  source.y += 460.0f;
  source.height = 620.0f;

  // Visual Scale:
  // Hitbox is ~100px.
  // We want the 620px content to Scale down to 100px.
  // If we draw to a 100px dest, it works.
  // But maybe we want it slightly larger (1.1x)?

  float visualScale = 1.1f;
  float drawWidth = fabs(source.width) * (100.0f / source.height) *
                    visualScale; // Maintain Aspect Ratio
  // Wait, source.width (frame width) is large (2360?). Content is probably much
  // narrower. Let's assume content width scales with height. Actually, we
  // should just let Raylib handle aspect ratio if we define dest width/height?
  // No, we need to know width to center it.

  // Let's rely on the frame width being roughly correct for the character
  // width? If frame width is 2360, that's huge. Log said "Crop 884,460
  // 12323x618". 12323 / 6 frames = 2053 width. The character is definitely not
  // 2000px wide if height is 600. The character is prob ~400px wide. Drawing
  // 2000px width into a small box will squash it if we don't crop X. Ideally we
  // crop X too, but X is animated. Let's scale based on HEIGHT.

  float drawHeight = player.size.y * visualScale; // Match hitbox height
  drawWidth = fabs(source.width) * (drawHeight / source.height);

  // Center alignment
  Rectangle destRect = {player.position.x - drawWidth / 2.0f,
                        player.position.y - drawHeight, // Align bottom
                        drawWidth, drawHeight};

  // Draw the specific frame from the sprite sheet with trimmed source
  DrawTexturePro(player.texture, source, destRect, {0, 0}, 0.0f, WHITE);
}
