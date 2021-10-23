#pragma once

#include "Commons.h"

class TextureAtlas;
class Core;

class GameEntity : GameObject
{
public:
	GameEntity(Core* core, TextureAtlas* atlas, Texture2D texture);

	virtual void Create() = 0;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;
	virtual void Destroy() = 0;

	UID GetUID();
protected:
	const UID m_uid;

	TextureAtlas* m_atlas;
	Core* m_core;
	Rectangle m_atlasRegion;
	Texture2D m_texture;

	Vec2f m_size;

	Vec2f m_position;
	Vec2f m_velocity;
	Vec2f m_acceleration;

	bool m_flipX;
	bool m_flipY;

	float m_angle;
	float m_angularVelocity;
	float m_angularAcceleration;
};

