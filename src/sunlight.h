#ifndef SUNLIGHT_H
#define SUNLIGHT_H

#include "raylib.h"
bool IsInSunlight(BoundingBox player, BoundingBox obstacle, Ray sunRay)
{
    RayCollision playerHit = GetRayCollisionBox(sunRay, player);
    RayCollision obstacleHit = GetRayCollisionBox(sunRay, obstacle);

    // If the ray didn't even hit the player's column, we are safe (or missed)
    if (!playerHit.hit) return false;

    // If we hit player, but MISSED the obstacle completely, we are definitely burning
    if (!obstacleHit.hit) return true;

    // If we hit BOTH, we check who is closer to the sun source.
    // If player distance < obstacle distance, player is closer to sun -> Burning.
    // If obstacle distance < player distance, obstacle is closer -> Shadow/Safe.
    return (playerHit.distance < obstacleHit.distance);
}

#endif