#include "Player.h"

Player::Player(Core* core) :
	Mob::Mob(core)
{
}

void Player::Update(float deltaTime)
{
	Vec2f direction = Vec2f(0.0f, 0.0f);

	if (IsKeyDown(KEY_D)) direction.SetX(direction.GetX() + 1.0f);
	if (IsKeyDown(KEY_A)) direction.SetX(direction.GetX() + -1.0f);

	m_velocity = direction * 80.0f;

	Mob::Update(deltaTime);
}