#include "Player.h"
#include "../../Render/TextureAtlas.h"

Player::Player(Core* core) :
	Mob::Mob(core)
{
	m_animTimer = Timer();
	m_animTimer.Reset();
	checksForMapCollisions = true;
}
int frame = 0;
void Player::Update(float deltaTime)
{
	float dir = 0.0f;

	if (IsKeyDown(KEY_D)) dir += 1.0f;
	if (IsKeyDown(KEY_A)) dir -= 1.0f;

	velocity.x = dir * 160.0f;

	bool isIdle = velocity.LengthSquared() <= 0.0001f;

	if (m_animTimer.TimeElapsed() >= (isIdle ? 0.2 : 0.1))
	{
		frame += 1;
		m_animTimer.Reset();
	}

	if (isIdle)
	{
		atlasRegion = atlas->GetAnimFrameRegion("wizzard_m_idle_anim", frame);
	}
	else
	{
		atlasRegion = atlas->GetAnimFrameRegion("wizzard_m_run_anim", frame);
	}

	Mob::Update(deltaTime);

	if (IsKeyDown(KEY_W) && velocity.y == 0.0f)
	{
		velocity.y = -860.0f;
	}
}