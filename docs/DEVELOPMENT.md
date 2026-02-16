# Development Guide

## Project Structure
The project is modularized into several key components located in `src/`:

- **Game Logic**:
    - `Game`: The core class managing the game loop, states (`TITLE`, `GAMEPLAY`, etc.), and resources.
    - `Level`: Manages level data including platforms, enemies, and background.
    - `Player`: Handles player movement, physics, and rendering.
    - `Cloud`: Handles cloud movement.
    - `Platform` & `Enemy`: Structs and logic for game objects.

- **Entry Point**: `main.cpp` simply instantiates `Game` and calls `Run()`.

## Modifying Game Data

### Adding/Editing Levels
Levels are initialized in `src/level.cpp` inside the `InitLevels()` function. We use helper methods to make this easy.

#### 1. Adding Platforms
Use `AddPlatform(x, y, width, height, type)`:
```cpp
// Example: Add a mushroom platform at coords (400, 500)
lvl.AddPlatform(400, 500, 100, 20, mushroom); 
```
Types: `basic`, `mushroom` (bouncy), `flower`, `invisible`.

#### 2. Adding Enemies
Use `AddEnemy(x, y, width, height, patrolSide, patrolIndex, type)`:
```cpp
// Example: Add a spider on the platform at index 2
lvl.AddEnemy(x, y, 100, 100, false, 2, spider); 
```

#### 3. Adding Clouds
Use `AddCloud(x, y, width, height, speed, leftLimit, rightLimit, movingRight)`:
```cpp
// Example: Cloud moving at speed 75.0f between x=200 and x=800
lvl.AddCloud(100, 50, 150, 40, 75.0f, 200, 800, true);
```
**To change speed**: Just modify the `speed` parameter (5th argument).

#### 4. Changing Level Textures
You can set custom textures for each level (Basic, Mushroom, Flower):
```cpp
lvl.texBasic = LoadTexture("custom_platform.png");
lvl.texMushroom = LoadTexture("another_shroom.png");
```
By default, they load `platform_basic.png`, `platform_mushroom.png`, etc.

### Changing Player Physics
Open `src/player.cpp` and look for the `#define` constants at the top:
```cpp
#define G 800           // Gravity
#define JUMP_SPEED 600.0f
#define MOVEMENT 200.0f // Horizontal speed
```

## Compilation
To rebuild the game after changes:
```bash
make
```
The executable will be in `bin/Debug/RenoDeAragon`.
