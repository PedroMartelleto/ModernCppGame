#include "Mob.h"
#include "Core.h"
#include "TextureAtlas.h"

Mob::Mob(Core* core) :
	GameEntity(core, core->GetTextureAtlas(), Texture2D())
{
}

void Mob::Create()
{
	m_texture = m_core->GetTextureManager()->Get("DungeonTileset/Atlas.png");
	m_atlasRegion = m_atlas->GetAnimFrameRegion("knight_f_idle_anim", 0);
	m_size = Vec2f(m_atlasRegion.width, m_atlasRegion.height) * 3.0f;
}

void Mob::Update(float deltaTime)
{
	GameEntity::Update(deltaTime);

	if (m_velocity.GetX() > 0.0f)
	{
		m_flipX = false;
	}
	else if (m_velocity.GetX() < 0.0f)
	{
		m_flipX = true;
	}
}

void Mob::Render()
{
	GameEntity::Render();
}

void Mob::Destroy()
{
}
