#pragma once

#include "Mob.h"

class Player : public Mob
{
private:
	Timer m_animTimer;
public:
	Player(Core* core);

	void Update(float deltaTime) override;
};