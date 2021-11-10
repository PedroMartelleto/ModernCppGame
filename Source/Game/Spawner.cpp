#include "Spawner.h"
#include "GameCore.h"
#include "GameData.h"

namespace Spawner
{
	b2Body* CreateDynamicBoxBody(GameCore* gameCore, const Vec2f& position, const Vec2f& size, const Vec2f& footRatio, SensorComponent* sensorComponent)
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(position.x * Game::PIXELS_TO_METERS, position.y * Game::PIXELS_TO_METERS);
		bodyDef.fixedRotation = true;
		bodyDef.allowSleep = false;

		b2PolygonShape box;
		box.SetAsBox(Game::PIXELS_TO_METERS * size.x / 2, Game::PIXELS_TO_METERS * size.y / 2);
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &box;
		fixtureDef.friction = 0.0f;
		fixtureDef.density = 1.0f;

		auto footSize = size * footRatio;

		b2PolygonShape footShape;
		footShape.SetAsBox(Game::PIXELS_TO_METERS * footSize.x / 2, Game::PIXELS_TO_METERS * footSize.y / 2, b2Vec2(0, Game::PIXELS_TO_METERS * (size.y / 2 + 1.0f)), 0.0f);

		b2Body* body = gameCore->physicsWorld.CreateBody(&bodyDef);
		body->CreateFixture(&fixtureDef);

		if (sensorComponent != nullptr)
		{
			b2FixtureDef footFixDef;
			footFixDef.shape = &footShape;
			footFixDef.isSensor = true;
			gameCore->DefineFixtureData(&footFixDef, new FixtureUserData{ sensorComponent });
			body->CreateFixture(&footFixDef);
		}

		return body;
	}

	void SpawnPlayer(GameCore* gameCore, MobID playerID, const std::string& charName, const Vec2f& tilePos, bool isLocal)
	{
		auto& registry = gameCore->registry;
		auto map = gameCore->map;

		if (map == nullptr)
		{
			DEBUG_LOG("CORE", LOG_ERROR, "Map cannot be nullptr when SpawnPlayer is called.");
			return;
		}

		auto player = gameCore->registry.create();

		auto mobComponent = GameData::GetMobData(charName, playerID);
		registry.emplace<MobComponent>(player, mobComponent);

		auto region = gameCore->atlas->GetAnimFrameRegion(charName + "_m_idle_anim", 0);
		auto aabbSize = Vec2f(9, 16);

		gameCore->registry.emplace<SensorComponent>(player);

		auto* sensorComponent = registry.try_get<SensorComponent>(player);
		auto dynamicBody = CreateDynamicBoxBody(gameCore, tilePos * map->scaledTileSize, aabbSize * Game::MAP_SCALE,
			Vec2f(0.9f, 0.2f), sensorComponent);

		registry.emplace<PhysicsBodyComponent>(player, dynamicBody);
		registry.emplace<SpriteComponent>(player, gameCore->textureManager->Get("DungeonTileset/Atlas.png"), 100,
			(Vec2f(-0.3f, -5) - region.size / 2.0f) * Game::MAP_SCALE, region.size * Game::MAP_SCALE, Colors::WHITE);
		registry.emplace<TextureRegionComponent>(player, region);

		registry.emplace<ProjectileInventoryComponent>(player, ProjectileInventoryComponent{ { GameData::GetProjectileData("arrow") }});

		if (isLocal)
		{
			auto input = GameData::CreateDefaultBinding(gameCore->localPlayerCount);
			registry.emplace<LocalInputComponent>(player, input);
			gameCore->localPlayerCount += 1;
		}

		gameCore->mobs[mobComponent.mobID] = player;
	}
}