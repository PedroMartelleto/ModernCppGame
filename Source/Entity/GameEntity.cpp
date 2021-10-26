#include "GameEntity.h"
#include "../Core/Core.h"
#include "../Render/TextureAtlas.h"
#include "../Map/TileMap.h"
#include <assert.h>

const float GameEntity::gravity = 9.8f;
const float GameEntity::jumpFallGravityMultiplier = 2.3f;

GameEntity::GameEntity(Core* core, TextureAtlas* atlas, Texture2D texture) :
	uid(core->CreateUID()),
	flipX(false),
	flipY(false),
	loopsInScreen(false),
	collisionState(CollisionState::Undefined),
	drawOffset(Vec2f::zero)
{
	this->atlas = atlas;
	this->core = core;
	this->texture = texture;
	this->atlasRegion = CreateRectangle(Vec2f::zero, Vec2f::one);
}

void GameEntity::CreateBody(b2BodyDef* bodyDef, b2FixtureDef* fixtureDef)
{
	body = core->physicsWorld.CreateBody(bodyDef);
	body->CreateFixture(fixtureDef);
}

void GameEntity::Update(float deltaTime)
{
	auto map = core->map;

	if (loopsInScreen && map != nullptr)
	{
		auto drawPos = Vec2f(body->GetPosition()) * METERS_TO_PIXELS + drawOffset * map->mapScale;
		auto width = map->layers[0]->GetWidth() * (float) map->layers[0]->GetTileset()->tileWidth * map->mapScale;
		auto height = map->layers[0]->GetHeight() * (float) map->layers[0]->GetTileset()->tileHeight * map->mapScale;
		
		if (drawPos.x + drawSize.x / 2 < 0 && body->GetLinearVelocity().x < 0.0f)
		{
			auto pos = body->GetPosition();
			pos.x = (width + drawSize.x/2) * PIXELS_TO_METERS;
			body->SetTransform(pos, body->GetAngle());
		}
		else if (drawPos.x - drawSize.x / 2 > width && body->GetLinearVelocity().x > 0.0f)
		{
			auto pos = body->GetPosition();
			pos.x = -drawSize.x/2 * PIXELS_TO_METERS;
			body->SetTransform(pos, body->GetAngle());
		}
		else if (drawPos.y + drawSize.y / 2 < 0 && body->GetLinearVelocity().y < 0.0f)
		{
			auto pos = body->GetPosition();
			pos.y = (height + drawSize.y / 2) * PIXELS_TO_METERS;
			body->SetTransform(pos, body->GetAngle());
		}
		else if (drawPos.y - drawSize.y / 2 > height && body->GetLinearVelocity().y > 0.0f)
		{
			auto pos = body->GetPosition();
			pos.y = -drawSize.y / 2 * PIXELS_TO_METERS;
			body->SetTransform(pos, body->GetAngle());
		}
	}
}

void GameEntity::Render()
{
	auto map = core->map;
	if (map == nullptr) return;

	drawSize = Vec2f(map->mapScale * atlasRegion.width, map->mapScale * atlasRegion.height);
	auto drawPos = Vec2f(body->GetPosition()) * METERS_TO_PIXELS + drawOffset * map->mapScale;
	auto renderDest = CreateRectangle(drawPos, drawSize);
	
	atlasRegion.width = flipX ? -fabsf(atlasRegion.width) : fabsf(atlasRegion.width);
	atlasRegion.height = flipY ? -fabsf(atlasRegion.height) : fabsf(atlasRegion.height);

	DrawTexturePro(texture, atlasRegion, renderDest, (drawSize / 2).raylib(), ToDegrees(body->GetAngle()), WHITE);
}