#ifndef CLOUD_H
#define CLOUD_H

#include "raylib.h"

typedef struct Cloud {
  Rectangle rect;
  float speed;
  float leftLimit;
  float rightLimit;
  bool movingRight;
} Cloud;

void UpdateCloud(Cloud &cloud, float delta);

#endif
