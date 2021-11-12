#include "Spawner.h"
#include "GameCore.h"
#include "GameData.h"
#include "Physics/WorldContactListener.h"

FixtureUserData* CreateFixtureUserData(entt::registry& registry, entt::entity entity)
{
	return new FixtureUserData{
		registry.try_get<SensorComponent>(entity), registry.try_get<ProjectileComponent>(entity), registry.try_get<MobComponent>(entity)
	};
}

namespace Spawner
{
	b2Body* CreateDynamicBoxBody(GameCore* gameCore, float density, const Vec2f& position, const Vec2f& size, const SensorPlacement& sensorPlacement,
								 const Vec2f& sensorRatio, FixtureUserData* fixtureUserData)
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
		fixtureDef.density = density;

		auto sensorSize = size * sensorRatio;

		b2Body* body = gameCore->physicsWorld.CreateBody(&bodyDef);
		body->CreateFixture(&fixtureDef);

		if (fixtureUserData != nullptr)
		{
			b2PolygonShape sensorShape;
			sensorShape.SetAsBox(Game::PIXELS_TO_METERS * sensorSize.x / 2, Game::PIXELS_TO_METERS * sensorSize.y / 2,
				b2Vec2(0, Game::PIXELS_TO_METERS * (size.y / 2 + 2.0f) * (sensorPlacement == SensorPlacement::Bottom ? 1 : -1)), 0.0f);

			b2FixtureDef sensorDef;
			sensorDef.shape = &sensorShape;
			sensorDef.isSensor = true;
			gameCore->DefineFixtureData(&sensorDef, fixtureUserData);
			body->CreateFixture(&sensorDef);
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

		registry.emplace<SensorComponent>(player);

		auto dynamicBody = CreateDynamicBoxBody(gameCore, mobComponent.density, tilePos * map->scaledTileSize,
			mobComponent.GetAABB().size() * map->mapScale, SensorPlacement::Bottom, Vec2f(0.9f, 0.2f),
			CreateFixtureUserData(registry, player)
		);

		registry.emplace<PhysicsBodyComponent>(player, dynamicBody);
		registry.emplace<SpriteComponent>(player, gameCore->textureManager->Get("DungeonTileset/Atlas.png"), 100,
			-(mobComponent.GetAABB().pos() + region.size()/2.0f) * map->mapScale, region.size() * map->mapScale, Colors::WHITE);
		registry.emplace<TextureRegionComponent>(player, region);

		std::vector<ProjectileData> temp;
		for (int i = 0; i < 50; ++i)
		{
			temp.push_back(rand() % 2 == 0 ? GameData::GetProjectileData("arrow") : GameData::GetProjectileData("sword"));
		}
		registry.emplace<ProjectileInventoryComponent>(player, ProjectileInventoryComponent{ temp });

		if (isLocal)
		{
			auto input = GameData::CreateDefaultBinding(gameCore->localPlayerCount);
			registry.emplace<LocalInputComponent>(player, input);
			gameCore->localPlayerCount += 1;
		}

		gameCore->mobs[mobComponent.mobID] = player;
	}

	entt::entity SpawnProjectile(GameCore* gameCore, const Vec2f& pos, const ProjectileData& projectileData)
	{
		auto& registry = gameCore->registry;
		auto map = gameCore->map;

		if (map == nullptr)
		{
			DEBUG_LOG("CORE", LOG_ERROR, "Map cannot be nullptr when SpawnProjectile is called.");
			return entt::null;
		}

		auto entity = gameCore->registry.create();
		
		registry.emplace<ProjectileComponent>(entity, projectileData);
		registry.emplace<SensorComponent>(entity);

		auto region = gameCore->atlas->GetRegion(projectileData.flyingSpriteName);
		auto dynamicBody = CreateDynamicBoxBody(
			gameCore, projectileData.density, pos, projectileData.GetAABB().size() * map->mapScale,
			SensorPlacement::Top, Vec2f(1.1f, 0.3f), CreateFixtureUserData(registry, entity));
		auto drawSize = region.size() * map->mapScale;

		registry.emplace<PhysicsBodyComponent>(entity, dynamicBody);
		registry.emplace<SpriteComponent>(entity, gameCore->textureManager->Get("DungeonTileset/Atlas.png"),
			100, -(projectileData.GetAABB().pos() * map->mapScale + drawSize/2.0f), drawSize, Colors::WHITE);
		registry.emplace<TextureRegionComponent>(entity, region);

		return entity;
	}
}