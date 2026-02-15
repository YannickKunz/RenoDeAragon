#include "platform.h"

void drawPlatform(Platform &platform) {
  Color color;
  if (platform.type == mushroom) {
    color = MAROON;
  } else if (platform.type == flower) {
    color = LIME;
  } else {
    color = GRAY;
  }

  DrawRectangleRec(platform.position, color);
}
