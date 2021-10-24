#include "Player.h"
#include "../../Render/TextureAtlas.h"

Player::Player(Core* core) :
	Mob::Mob(core)
{
	m_animTimer = Timer();
	m_animTimer.Reset();
}
int frame = 0;
void Player::Update(float deltaTime)
{
	Vec2f direction = Vec2f(0.0f, 0.0f);

	if (IsKeyDown(KEY_D)) direction.SetX(direction.GetX() + 1.0f);
	if (IsKeyDown(KEY_A)) direction.SetX(direction.GetX() + -1.0f);

	m_velocity = direction * 80.0f;

	bool isIdle = m_velocity.LengthSq() <= 0.0001f;

	if (m_animTimer.TimeElapsed() >= (isIdle ? 0.2 : 0.1))
	{
		frame += 1;
		m_animTimer.Reset();
	}

	if (isIdle)
	{
		m_atlasRegion = m_atlas->GetAnimFrameRegion("wizzard_m_idle_anim", frame);
	}
	else
	{
		m_atlasRegion = m_atlas->GetAnimFrameRegion("wizzard_m_run_anim", frame);
	}

	Mob::Update(deltaTime);
}