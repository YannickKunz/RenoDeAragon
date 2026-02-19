# Game Architecture & Logic Reference

## 1. Directory Structure
Recommended organization to keep Logic (Physics) separate from Data (Assets) and Rendering.

```text
MyGame/
├── assets/                     # Game Assets
│   ├── sprites/                # .png files (Character, Enemies, Items)
│   ├── audio/                  # .wav/.mp3 files (Music, SFX)
│   └── levels/                 # (Optional) Text files or JSON for map data
│
├── src/                        # Source Code
│   ├── main.cpp                # Entry point: Window Init & Main Game Loop
│   ├── Game.h                  # Main Game Class (Manages state, level loading)
│   ├── Game.cpp
│
│   ├── core/                   # Global Utilities
│   │   ├── Constants.h         # Gravity, Screen Size, Speed constants
│   │   └── Types.h             # Enums (DayNightState, Direction) & Structs
│   │
│   ├── entities/               # Dynamic Objects (Move/Act)
│   │   ├── Entity.h            # Base Class (Position, Size, Texture)
│   │   ├── Player.h            # Player specific physics & input
│   │   ├── Player.cpp
│   │   ├── Enemy.h             # Base Enemy Class (Patrol Logic)
│   │   ├── Spider.cpp          # Spider specific overrides
│   │   └── Roach.cpp           # Roach specific overrides
│   │
│   └── world/                  # Static/Environmental Objects
│       ├── Level.h             # Tilemap/Platform manager
│       ├── Level.cpp
│       ├── Platform.h          # Logic for solid ground
│       ├── Mushroom.cpp        # Logic for Bouncy Platforms (Night)
│       └── Flower.cpp          # Logic for Day Platforms
│
├── CMakeLists.txt              # Build configuration
└── README.md

```

---

## 2. Improved Physics Logic (Axis Separation)

To prevent "corner snagging" and "teleporting" through walls, we must update the X axis and Y axis separately.

**The Golden Rule:** Move X  Check X Collision  Move Y  Check Y Collision.

### Core Update Loop (Pseudocode for `Player.cpp`)

```cpp
void Player::Update(float delta, std::vector<Platform>& platforms) {
    
    // --- 1. HORIZONTAL PASS ---
    // Apply Velocity
    position.x += velocity.x * delta;

    // Check Collisions
    Rectangle playerRect = GetRect(); 
    for (auto& plat : platforms) {
        if (!plat.IsActive()) continue; // Skip inactive flowers/mushrooms

        if (CheckCollisionRecs(playerRect, plat.rect)) {
            // Collision Detected on X Axis
            if (velocity.x > 0) {
                // Moving Right -> Hit Left Wall
                position.x = plat.rect.x - size.x; 
            }
            else if (velocity.x < 0) {
                // Moving Left -> Hit Right Wall
                position.x = plat.rect.x + plat.rect.width;
            }
            velocity.x = 0; // Stop momentum
        }
    }

    // --- 2. VERTICAL PASS ---
    // Apply Gravity & Velocity
    velocity.y += GRAVITY * delta;
    position.y += velocity.y * delta;

    // Check Collisions
    playerRect = GetRect(); // Update rect with new Y position
    isGrounded = false;     // Assume falling until proven otherwise

    for (auto& plat : platforms) {
        if (!plat.IsActive()) continue;

        if (CheckCollisionRecs(playerRect, plat.rect)) {
            // Collision Detected on Y Axis
            if (velocity.y > 0) {
                // Falling Down -> Hit Floor
                position.y = plat.rect.y - size.y;
                velocity.y = 0;
                isGrounded = true;

                // SPECIAL LOGIC: MUSHROOM
                if (plat.type == PlatformType::MUSHROOM) {
                    velocity.y = -JUMP_FORCE * 1.5f; // Bounce!
                    isGrounded = false;
                }
            }
            else if (velocity.y < 0) {
                // Jumping Up -> Hit Ceiling
                position.y = plat.rect.y + plat.rect.height;
                velocity.y = 0;
            }
        }
    }
}

```

---

## 3. Class Hierarchy & Logic

### A. The Entity System (Enemies)

Don't write unique code for every enemy. Use inheritance.

* **`Entity` (Base):** Holds `Vector2 position`, `int hp`, `Texture2D sprite`.
* **`Enemy` (Inherits Entity):** Adds `Patrol()` function.
* **`Spider` / `Roach` (Inherits Enemy):** Overrides `Update()`.

**Example: `Enemy.h**`

```cpp
class Enemy {
public:
    Vector2 pos;
    float speed;
    bool movingRight;

    virtual void Update(float dt, const Level& level) {
        // 1. Generic Patrol Logic
        float move = speed * dt;
        if (movingRight) pos.x += move;
        else pos.x -= move;

        // 2. Turn around at edges (checked against level data)
        if (level.IsEdge(pos)) movingRight = !movingRight;
    }
    
    // Virtual allows Roach/Spider to have custom drawing or attacks
    virtual void Draw() = 0; 
};

```

### B. The World System (Platforms)

Platforms need to change based on the Day/Night cycle.

**Example: `Platform.h**`

```cpp
enum PlatformType { NORMAL, FLOWER, MUSHROOM };

struct Platform {
    Rectangle rect;
    PlatformType type;

    // The central logic for your mechanic
    bool IsSolid(bool isDayTime) {
        if (type == NORMAL) return true;
        if (type == FLOWER && isDayTime) return true;
        if (type == MUSHROOM && !isDayTime) return true; // Only solid at night
        return false;
    }
};

```

---

## 4. Game Manager (The "Brain")

In `src/Game.h`, this class holds the lists of objects. This prevents your `main.cpp` from getting messy.

```cpp
class Game {
public:
    void Init();
    void Update();
    void Draw();

private:
    bool isDayTime;
    float dayTimer;

    Player player;
    std::vector<Platform> platforms;
    std::vector<Enemy*> enemies; // Use pointers for polymorphism
    
    // Level Management
    void LoadLevel(int levelIndex);
};

```

## 5. Implementation Roadmap

1. **Phase 1 (Skeleton):** Create the folder structure and the `Game` class. Get a window opening.
2. **Phase 2 (World):** Implement `Platform` struct and `Level` loading. Draw rectangles for ground.
3. **Phase 3 (Player):** Implement `Player.cpp` with the **Axis Separation** physics shown above.
4. **Phase 4 (Mechanic):** Add the Day/Night toggle variable. Connect it to `Platform::IsSolid()`.
5. **Phase 5 (Enemies):** Create the `Enemy` base class and add one simple patrolling square.
6. **Phase 6 (Juice):** Replace rectangles with your sprites/textures and add sound.
