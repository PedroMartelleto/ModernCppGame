#include "Player.h"

Player::Player(Core* core) :
	Mob::Mob(core, nullptr)
{
	horizontalDirection = 0;

	m_animTimer = Timer();
	m_animTimer.Reset();

	loopsInScreen = true;

	drawOffset = Vec2f(-1, -4);

	movementImpulse = 15.0f;
	maxMoveSpeed = 12.0f;
	linearDrag = 10.0f;
}

void Player::Create()
{
	auto mapScale = core->map->mapScale;

	b2BodyDef bodyDef;
	bodyDef.fixedRotation = true;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(440.0f * PIXELS_TO_METERS, 100.0f * PIXELS_TO_METERS);
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(6.0f * mapScale * PIXELS_TO_METERS, 9.0f * mapScale * PIXELS_TO_METERS);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.01f;

	CreateBody(&bodyDef, &fixtureDef);
	Mob::Create();
}

void Player::HandleInputs(float deltaTime)
{
	if (body == nullptr) return;

	horizontalDirection = 0.0f;

	if (IsKeyDown(KEY_D)) horizontalDirection += 1.0f;
	if (IsKeyDown(KEY_A)) horizontalDirection -= 1.0f;

	if (horizontalDirection != 0.0f)
	{
		body->ApplyForceToCenter(b2Vec2(movementImpulse * horizontalDirection, 0), true);
	}

	auto velocity = body->GetLinearVelocity();
	if (velocity.x > maxMoveSpeed)
	{
		velocity.x = maxMoveSpeed;
		body->SetLinearVelocity(velocity);
	}

	if (IsKeyDown(KEY_W))
	{
		body->ApplyForce(b2Vec2(0, -32), body->GetPosition(), true);
	}
}

int frame = 0;

void Player::UpdateAnimations(float deltaTime)
{
	bool isIdle = body->GetLinearVelocity().LengthSquared() <= 0.0001f;

	if (m_animTimer.TimeElapsed() >= (isIdle ? 0.2 : 0.1))
	{
		frame += 1;
		m_animTimer.Reset();
	}

	if (isIdle)
	{
		atlasRegion = atlas->GetAnimFrameRegion("knight_m_idle_anim", frame);
	}
	else
	{
		atlasRegion = atlas->GetAnimFrameRegion("knight_m_run_anim", frame);
	}
}

void Player::UpdateState(float deltaTime)
{
	if (core == nullptr || core->map == nullptr) return;

	// auto map = core->map;
	// auto mapBody = map->body;
}

void Player::Update(float deltaTime)
{
	UpdateState(deltaTime);
	HandleInputs(deltaTime);
	UpdateAnimations(deltaTime);

	Mob::Update(deltaTime);
}