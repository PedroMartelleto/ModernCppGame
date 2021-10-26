#include "Mob.h"
#include "../../Core/Core.h"
#include "../../Render/TextureAtlas.h"

Mob::Mob(Core* core) :
	GameEntity(core, core->GetTextureAtlas(), Texture2D())
{
	applyGravity = true;
}

void Mob::Create()
{
	texture = core->GetTextureManager()->Get("DungeonTileset/Atlas.png");
	atlasRegion = atlas->GetAnimFrameRegion("knight_f_idle_anim", 0);
	size = Vec2f(atlasRegion.width, atlasRegion.height) * 3.0f;
}

void Mob::Update(float deltaTime)
{
	GameEntity::Update(deltaTime);

	if (velocity.x > 0.0f)
	{
		flipX = false;
	}
	else if (velocity.x < 0.0f)
	{
		flipX = true;
	}
}

void Mob::Render()
{
	GameEntity::Render();
}

void Mob::Destroy()
{
}
