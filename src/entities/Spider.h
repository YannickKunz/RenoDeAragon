#pragma once
#include "Enemy.h"

class Spider : public Enemy {
public:
  Spider(Vector2 pos) : Enemy(pos) {
    speed = 100.0f; // Slower patrol speed
    movingRight = true;
  }

  void Update(float dt, const Level &level) override;
  void Draw() override;

private:
  bool movingRight;
};
