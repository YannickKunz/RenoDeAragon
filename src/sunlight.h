#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "star_donut.h"	// our star donut demo code

bool IsInSunlight(BoundingBox player, BoundingBox obstacle, Ray sunRay)
{
    RayCollision playerHit = GetRayCollisionBox(sunRay, player);
    RayCollision obstacleHit = GetRayCollisionBox(sunRay, obstacle);

    if (playerHit.hit)
    {
        if (obstacleHit.hit)
        {
            // If the obstacle is hit before the player, then the player is in shadow
            return playerHit.distance < obstacleHit.distance;
        }
        // If the player is hit and the obstacle is not, then the player is in sunlight
        return true;
    }

}