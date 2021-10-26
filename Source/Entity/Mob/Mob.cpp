#include "Mob.h"
#include "../../Core/Core.h"
#include "../../Render/TextureAtlas.h"
#include "../../Map/TileMap.h"

Mob::Mob(Core* core, b2BodyDef* bodyDef) :
	GameEntity(core, core->GetTextureAtlas(), Texture2D())
{
}

void Mob::Create()
{
	texture = core->GetTextureManager()->Get("DungeonTileset/Atlas.png");
	atlasRegion = atlas->GetAnimFrameRegion("knight_f_idle_anim", 0);

	GameEntity::Create();
}

void Mob::Update(float deltaTime)
{
	GameEntity::Update(deltaTime);

	if (body == nullptr) return;

	auto velocity = body->GetLinearVelocity();

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
