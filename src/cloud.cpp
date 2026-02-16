#include "cloud.h"

void UpdateCloud(Cloud &cloud, float delta) {
  float moveAmount = cloud.speed * delta;
  if (cloud.movingRight) {
    cloud.rect.x += moveAmount;
    if (cloud.rect.x > cloud.rightLimit)
      cloud.movingRight = false;
  } else {
    cloud.rect.x -= moveAmount;
    if (cloud.rect.x < cloud.leftLimit)
      cloud.movingRight = true;
  }
}
