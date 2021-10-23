#pragma once

#include "Mob.h"

class Player : public Mob
{
public:
	Player(Core* core);

	void Update(float deltaTime) override;
};