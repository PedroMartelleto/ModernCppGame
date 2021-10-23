#include "GameEntity.h"
#include "../Core/Core.h"
#include "../Render/TextureAtlas.h"

GameEntity::GameEntity(Core* core, TextureAtlas* atlas, Texture2D texture) :
	m_uid(core->CreateUID())
{
	m_atlas = atlas;
	m_core = core;
	m_texture = texture;
	m_position = Vec2f::zero;
	m_velocity = Vec2f::zero;
	m_acceleration = Vec2f::zero;
	m_size = Vec2f::one * 16.0f;
	m_atlasRegion = CreateRectangle(0, 0, 16, 16);
	m_angle = 0.0f;
	m_angularVelocity = 0.0f;
	m_angularAcceleration = 0.0f;
	m_flipX = false;
	m_flipY = false;
}

UID GameEntity::GetUID()
{
	return m_uid;
}

void GameEntity::Update(float deltaTime)
{
	m_velocity += m_acceleration * deltaTime;
	m_position += m_velocity * deltaTime;

	m_angularVelocity += m_angularAcceleration * deltaTime;
	m_angle += m_angularVelocity * deltaTime;
}

void GameEntity::Render()
{
	auto renderDest = CreateRectangle(m_position.GetX(), m_position.GetY(), m_size.GetX(), m_size.GetY());
	
	m_atlasRegion.width = m_flipX ? -fabsf(m_atlasRegion.width) : fabsf(m_atlasRegion.width);
	m_atlasRegion.height = m_flipY ? -fabsf(m_atlasRegion.height) : fabsf(m_atlasRegion.height);

	DrawTexturePro(m_texture, m_atlasRegion, renderDest, Vec2f(0, 0).raylib(), m_angle, WHITE);
}
