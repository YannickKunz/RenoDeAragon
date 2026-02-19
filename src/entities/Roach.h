#pragma once
#include "Enemy.h"

class Roach : public Enemy {
public:
  Roach(Vector2 pos) : Enemy(pos) { speed = 100.0f; }

  void Draw() override;
  void Update(float dt, const Level &level) override;
};
