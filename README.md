# MyGame

A game built with Raylib following a clean architecture separating Logic, Data, and Rendering.

## Structure
- **src/core**: Global constants and types.
- **src/entities**: Dynamic objects like Player and Enemies.
- **src/world**: Static objects like Platforms and Levels.
- **assets**: Game assets (sprites, audio).

## Building
```bash
mkdir build
cd build
cmake ..
make
./MyGame
```
