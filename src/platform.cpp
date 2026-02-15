#include "platform.h"
#include <iostream>

#define BLANK (Color){0, 0, 0, 0}

extern bool playerToggle;

bool isPlatformActive(Platform& platform) {
    switch (platform.type) {
        case mushroom: return !playerToggle;
        case flower:   return playerToggle;
        case invisible: return true; // Physical but unseen
        default:       return true;
    }
}

void drawPlatform(const Platform& platform, Texture2D mushroomTex, Texture2D flowerTex, Texture2D basicTex) {
    bool draw = true;
    Color color = GRAY;       // Default fallback color
    Texture2D texture = {0};  // Default invalid texture

    // 1. Determine properties based on type
    switch (platform.type) {
        case mushroom:
            color = MAROON;
            draw = !playerToggle;
            texture = mushroomTex;
            break;
        case flower:
            color = LIME;
            draw = playerToggle;
            texture = flowerTex;
            break;
        case invisible:
            color = BLANK;
            draw = false; // Never draw invisible
            break;
        default: // basic
            color = GRAY;
            draw = true;
            texture = basicTex;
            break;
    }

    // 2. Draw loop
    if (draw) {
        // OPTION A: If a valid texture exists, draw it stretched
        if (texture.id > 0) {
            Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
            Rectangle dest = platform.position;
            Vector2 origin = { 0.0f, 0.0f };
            
            DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
        } 
        // OPTION B: No texture? Draw the colored rectangle
        else {
            DrawRectangleRec(platform.position, color);
        }
    }
}
