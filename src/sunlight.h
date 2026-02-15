#ifndef SUNLIGHT_H
#define SUNLIGHT_H

#include "raylib.h"

bool IsInSunlight(BoundingBox player, BoundingBox obstacle, Ray sunRay);

#endif
