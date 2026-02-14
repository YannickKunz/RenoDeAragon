#ifndef STAR_DONUT_H
#define STAR_DONUT_H

#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define STAR_COUNT 420

// 1. Define a struct to hold the state so we don't use global variables
typedef struct {
    Vector3 stars[STAR_COUNT];
    float A;
    float B;
} StarDonutState;

// 2. Initialization Function
void InitStarDonut(StarDonutState* state, int screenWidth, int screenHeight) {
    state->A = 0;
    state->B = 0;
    for (int i = 0; i < STAR_COUNT; i++) {
        state->stars[i].x = GetRandomValue(-screenWidth * 0.5f, screenWidth * 0.5f);
        state->stars[i].y = GetRandomValue(-screenHeight * 0.5f, screenHeight * 0.5f);
        state->stars[i].z = (float)GetRandomValue(1, 100) / 100.0f;
    }
}

// 3. Draw/Update Function
void DrawStarDonut(StarDonutState* state, int screenWidth, int screenHeight) {
    // Constants
    const float theta_spacing = 0.07f;
    const float phi_spacing = 0.02f;
    const float R1 = 1;
    const float R2 = 2;
    const float K2 = 5;
    // Calculate K1 based on screen size used
    const float K1 = screenWidth * K2 * 3 / (8 * (R1 + R2));

    // Update rotations
    state->A += 0.04f;
    state->B += 0.02f;

    float dt = GetFrameTime();
    float speed = 0.5f;

    // --- 1. DRAW STARFIELD ---
    for (int i = 0; i < STAR_COUNT; i++) {
        state->stars[i].z -= dt * speed;
        if (state->stars[i].z <= 0) state->stars[i].z = 1.0f;

        Vector2 pos = {
            screenWidth * 0.5f + state->stars[i].x / state->stars[i].z,
            screenHeight * 0.5f + state->stars[i].y / state->stars[i].z
        };

        if (pos.x < 0 || pos.x > screenWidth || pos.y < 0 || pos.y > screenHeight) {
            state->stars[i].z = 1.0f;
        } else {
            DrawCircleV(pos, (1.0f - state->stars[i].z) * 2.0f, WHITE);
        }
    }

    // --- 2. DRAW DONUT ---
    float cosA = cosf(state->A), sinA = sinf(state->A);
    float cosB = cosf(state->B), sinB = sinf(state->B);

    for (float theta = 0; theta < 2 * PI; theta += theta_spacing) {
        float costheta = cosf(theta), sintheta = sinf(theta);

        for (float phi = 0; phi < 2 * PI; phi += phi_spacing) {
            float cosphi = cosf(phi), sinphi = sinf(phi);

            float circlex = R2 + R1 * costheta;
            float circley = R1 * sintheta;

            float x = circlex * (cosB * cosphi + sinA * sinB * sinphi) - circley * cosA * sinB;
            float y = circlex * (sinB * cosphi - sinA * cosB * sinphi) + circley * cosA * cosB;
            float z = K2 + cosA * circlex * sinphi + circley * sinA;
            float ooz = 1 / z; // one over z

            int xp = (int)(screenWidth / 2 + K1 * ooz * x);
            int yp = (int)(screenHeight / 2 - K1 * ooz * y);

            float L = cosphi * costheta * sinB - cosA * costheta * sinphi -
                      sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);

            if (L > 0) {
                int luminance_index = (int)(L * 8);
                char chars[] = ".,-~:;=!*#$@";
                int charIndex = luminance_index;
                if (charIndex < 0) charIndex = 0;
                if (charIndex > 11) charIndex = 11;
                
                char str[2] = { chars[charIndex], '\0' };
                DrawText(str, xp, yp, 10, WHITE);
            }
        }
    }
}

#endif // STAR_DONUT_H