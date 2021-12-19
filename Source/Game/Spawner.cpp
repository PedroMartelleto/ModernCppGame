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

		if (fixtureUserData != nullptr)
		{
			gameCore->DefineFixtureData(&fixtureDef, fixtureUserData);
		}

		auto sensorSize = size * sensorRatio;
		
		b2Body* body = gameCore->physicsWorld.CreateBody(&bodyDef);
		body->CreateFixture(&fixtureDef);

		if (fixtureUserData != nullptr)
		{
			b2PolygonShape sensorShape;
			sensorShape.SetAsBox(Game::PIXELS_TO_METERS * sensorSize.x / 2, Game::PIXELS_TO_METERS * sensorSize.y / 2,
				b2Vec2(0, Game::PIXELS_TO_METERS * (size.y / 2 + 6.0f) * (sensorPlacement == SensorPlacement::Bottom ? 1 : -1)), 0.0f);

			b2FixtureDef sensorDef;
			sensorDef.shape = &sensorShape;
			sensorDef.isSensor = true;
			sensorDef.userData.pointer = fixtureDef.userData.pointer;

			body->CreateFixture(&sensorDef);
		}

		return body;
	}

	entt::entity SpawnMob(GameCore* gameCore, MobID mobID, const std::string& charName, const Vec2f& pos)
	{
		auto& registry = gameCore->registry;
		auto map = gameCore->map;

		if (map == nullptr)
		{
			DEBUG_LOG("CORE", LOG_ERROR, "Map cannot be nullptr when SpawnMob is called.");
			return entt::null;
		}

		// Creates ECS entity and mob component
		auto entity = gameCore->registry.create();
		auto mobComponent = GameData::GetMobData(charName, mobID);
		registry.emplace<MobComponent>(entity, mobComponent);

		// Setups atlas
		auto atlas = gameCore->GetAtlas(mobComponent.atlas);
		auto& anim = registry.emplace<AnimationComponent>(entity, atlas, mobComponent.name + "_idle_anim");
		anim.tickRate = mobComponent.idleTickRate;
		auto region = anim.GetRect();

		// Setups physics and rendering
		registry.emplace<SensorComponent>(entity);

		auto dynamicBody = CreateDynamicBoxBody(gameCore, mobComponent.density, pos,
			mobComponent.GetAABB().size() * map->mapScale, SensorPlacement::Bottom, Vec2f(0.4f, 0.2f),
			CreateFixtureUserData(registry, entity)
		);

		registry.emplace<PhysicsBodyComponent>(entity, dynamicBody, mobComponent.dragMultiplier);
		registry.emplace<SpriteComponent>(entity, atlas->texture, ZPlanes::MOBS, -(mobComponent.GetAABB().pos() + region.size() / 2.0f) * map->mapScale,
										  region.size() * map->mapScale, Colors::WHITE);

		auto* inventory = mobComponent.initialAmmo > 0 ? &registry.emplace<ProjectileInventoryComponent>(entity) : nullptr;

		// Ready the mob component
		mobComponent.Reset(inventory);

		gameCore->mobs[mobComponent.mobID] = entity;

		return entity;
	}

	entt::entity SpawnEnemyMob(GameCore* gameCore, MobID mobID, const std::string& charName, const Vec2f& pos)
	{
		LOGGER_VAR(charName);
		auto entity = SpawnMob(gameCore, mobID, charName, pos);
		return entity;
	}

	entt::entity SpawnPlayer(GameCore* gameCore, MobID playerID, const std::string& charName, const Vec2f& pos, bool isLocal)
	{
		auto player = SpawnMob(gameCore, playerID, charName, pos);
		auto& registry = gameCore->registry;

		registry.get<MobComponent>(player).playerIndex = gameCore->CreatePlayerIndex();

		if (isLocal)
		{
			auto input = GameData::CreateDefaultBinding(gameCore->localPlayerCount);
			registry.emplace<LocalInputComponent>(player, input);
			gameCore->localPlayerCount += 1;
		}

		gameCore->players.push_back(player);

		return player;
	}

	entt::entity SpawnProjectile(GameCore* gameCore, entt::entity owner, const Vec2f& pos, const ProjectileData& projectileData)
	{
		auto& registry = gameCore->registry;
		auto map = gameCore->map;

		if (map == nullptr)
		{
			DEBUG_LOG("CORE", LOG_ERROR, "Map cannot be nullptr when SpawnProjectile is called.");
			return entt::null;
		}

		auto entity = gameCore->registry.create();
		
		auto& projComponent = registry.emplace<ProjectileComponent>(entity, entity, projectileData);
		projComponent.projectileData.owner = owner;
		registry.emplace<SensorComponent>(entity);

		auto atlas = gameCore->GetAtlas(projectileData.atlas);
		auto& regionComponent = registry.emplace<AnimationComponent>(entity, atlas, projectileData.flyingSpriteName);
		auto region = regionComponent.GetRect();
		
		auto* userData = CreateFixtureUserData(registry, entity);
		auto* dynamicBody = CreateDynamicBoxBody(
			gameCore, projectileData.density, pos, projectileData.GetAABB().size() * map->mapScale,
			SensorPlacement::Top, Vec2f(1.1f, 0.3f), userData);

		auto drawSize = region.size() * map->mapScale;

		registry.emplace<PhysicsBodyComponent>(entity, dynamicBody, projectileData.dragMultiplier);
		registry.emplace<SpriteComponent>(entity, atlas->texture, ZPlanes::PROJECTILES,
			-(projectileData.GetAABB().pos() * map->mapScale + drawSize/2.0f), drawSize, Colors::WHITE);

		return entity;
	}
}