#pragma once

#include "Mob.h"

class Player : public Mob
{
private:
	Timer m_animTimer;
public:
	Player(Core* core);

	void Create() override;
	void Update(float deltaTime) override;
public:
	float movementImpulse;
	float maxMoveSpeed;
	float linearDrag;
	float horizontalDirection;
private:
	void UpdateState(float deltaTime);
	void UpdateAnimations(float deltaTime);
	void HandleInputs(float deltaTime);
};