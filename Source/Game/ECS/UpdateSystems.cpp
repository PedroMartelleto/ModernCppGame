#include "UpdateSystems.h"
#include "../GameCore.h"
#include "../GameData.h"
#include "../Spawner.h"
#include "../../Engine/Pathfinding/AStarSearch.h"

std::unordered_map<MobID, AStarSearch::WorldGraphPath> shortestPaths;

namespace UpdateSystems
{
	void ECSMobPathfindingUpdateSystem(GameCore* gameCore, float deltaTime)
	{
		auto& registry = gameCore->registry;
		const auto& map = gameCore->map;
		double startTime = Timer::GetTime();

		auto& target = registry.get<PhysicsBodyComponent>(gameCore->mobs[1]);

		for (auto entity : registry.view<MobComponent, PhysicsBodyComponent, PathfindingComponent>())
		{
			auto& mob = registry.get<MobComponent>(entity);
			auto& pathfinding = registry.get<PathfindingComponent>(entity);
			auto* body = registry.get<PhysicsBodyComponent>(entity).body;

			auto mobPos = Vec2fFromB2((Game::METERS_TO_PIXELS / map->mapScale) * body->GetPosition());
			auto targetPos = Vec2fFromB2((Game::METERS_TO_PIXELS / map->mapScale) * target.body->GetPosition());

			auto& srcNode = map->pathfindingGraph.GetClosestNode(mobPos);
			auto& dstNode = map->pathfindingGraph.GetClosestNode(targetPos);
			srcNode.isMarked = true;
			dstNode.isMarked = true;

			if (srcNode.id == dstNode.id) continue;

			shortestPaths[mob.mobID] = AStarSearch::FindShortestPath(map->pathfindingGraph, srcNode.id, dstNode.id, [](const Vec2f& curr, const Vec2f& goal)
			{
				return 0.0f;
			});

			auto& nextNode = map->pathfindingGraph[shortestPaths[mob.mobID][srcNode.id]];

			for (auto& link : srcNode.links)
			{
				if (link.dst == nextNode.id)
				{
					link.isMarked = true;
				}
			}

			if (srcNode.worldPos.x < nextNode.worldPos.x)
			{
				mob.horizontalMoveDir = 1.0f;
			}
			else
			{
				mob.horizontalMoveDir = -1.0f;
			}

			if (srcNode.worldPos.y - nextNode.worldPos.y > 12.0f)
			{
				mob.wantsToJump = true;
			}
		}

		if (gameCore->frameCounter % 30 == 0)
		{
			DEBUG_LOG("AI", LOG_MSG, "Time to find paths: %lf ms", (Timer::GetTime() - startTime) * 1000);
		}
	}

	void ECSMobTextureRegionUpdateSystem(GameCore* gameCore, float deltaTime)
	{
		auto& registry = gameCore->registry;

		// Animates all texture regions
		for (auto entity : registry.view<MobComponent, AnimationComponent>())
		{
			auto& mob = registry.get<MobComponent>(entity);
			auto& anim = registry.get<AnimationComponent>(entity);

			if (fabsf(mob.horizontalMoveDir) > 0.3f)
			{
				anim.regionName = mob.name + "_run_anim";
				anim.tickRate = mob.runTickRate;
			}
			else
			{
				anim.regionName = mob.name + "_idle_anim";
				anim.tickRate = mob.idleTickRate;
			}

			if (anim.tickRate > 0)
			{
				if (gameCore->frameCounter % anim.tickRate == 0)
				{
					anim.frame += 1;

					if (anim.frame >= UINT8_MAX - 1)
					{
						anim.frame = 0;
					}
				}
			}
		}
	}

	void ECSBodyUpdateSystem(GameCore* gameCore, float deltaTime)
	{
		auto& registry = gameCore->registry;

		// Applies air drag to all moving bodies
		for (auto entity : registry.view<PhysicsBodyComponent>())
		{
			auto& body = registry.get<PhysicsBodyComponent>(entity);
			// Air drag
			float Cd = 1.05f;
			float v = body.body->GetLinearVelocity().x;

			if (fabsf(v) > 0.0001f)
			{
				auto appliedDrag = b2Vec2(-0.5f * v * Cd * body.dragMultiplier, 0);
				body.body->ApplyForceToCenter(appliedDrag, true);
			}
		}
	}

	void ECSMobBodyUpdateSystem(GameCore* gameCore, float deltaTime)
	{
		auto& registry = gameCore->registry;

		// Mob movement
		for (auto entity : registry.view<PhysicsBodyComponent, MobComponent>())
		{
			auto& body = registry.get<PhysicsBodyComponent>(entity);
			auto& mob = registry.get<MobComponent>(entity);

			if (mob.wantsToShoot && !mob.readyToShoot)
			{
				auto* inventory = registry.try_get<ProjectileInventoryComponent>(entity);

				if (inventory != nullptr && inventory->projectiles.size() > 0 && Timer::GetTime() - mob.lastShotTime >= 0.15)
				{
					auto projToShoot = inventory->projectiles.back();
					Projectile::SpawnAndShoot(gameCore, entity, projToShoot);
					inventory->projectiles.pop_back();
					mob.lastShotTime = Timer::GetTime();
					mob.wantsToShoot = false;
					mob.readyToShoot = false;
					mob.shootDirection.Reset();
				}
			}

			if (mob.wantsToJump)
			{
				auto* sensorComponent = registry.try_get<SensorComponent>(entity);

				if (sensorComponent != nullptr && sensorComponent->IsColliding())
				{
					auto impulse = b2Vec2(0.0f, -mob.jumpHeight * body.body->GetMass());
					body.body->ApplyLinearImpulse(impulse, b2Vec2(0, 0), true);
				}
			}

			// Movement impulse
			if (mob.horizontalMoveDir != 0.0f)
			{
				auto impulse = b2Vec2(mob.horizontalMoveDir * mob.horizontalImpulse, 0.0f);
				body.body->ApplyLinearImpulse(impulse, b2Vec2(0, 0), true);
			}

			// Velocity clamp
			auto velocity = body.body->GetLinearVelocity();
			velocity.x = Math::Clamp(velocity.x, -mob.maxHorizontalSpeed, mob.maxHorizontalSpeed);
			body.body->SetLinearVelocity(velocity);
		}
	}

	void ECSMobInputUpdateSystem(GameCore* gameCore, float deltaTime)
	{
		auto& registry = gameCore->registry;
		json bitBuffers;

		// Movevement of controllable entities
		for (auto entity : registry.view<LocalInputComponent, MobComponent>())
		{
			auto& input = registry.get<LocalInputComponent>(entity);
			auto& mob = registry.get<MobComponent>(entity);
			bool moveDown = Input::IsKeyDown(input.inputCodes[GameData::GetMobActionBit("MOVE_DOWN")]);

			mob.wantsToJump = Input::IsKeyDown(input.inputCodes[GameData::GetMobActionBit("MOVE_UP")]);
			mob.horizontalMoveDir = 0.0f;

			if (Input::IsKeyDown(input.inputCodes[GameData::GetMobActionBit("MOVE_LEFT")]))
			{
				mob.horizontalMoveDir -= 1.0f;
			}

			if (Input::IsKeyDown(input.inputCodes[GameData::GetMobActionBit("MOVE_RIGHT")]))
			{
				mob.horizontalMoveDir += 1.0f;
			}

			auto shootKey = input.inputCodes[GameData::GetMobActionBit("SHOOT_DOWN")];
			mob.readyToShoot = Input::IsKeyDown(shootKey);

			if (mob.readyToShoot)
			{
				auto l = mob.horizontalMoveDir < 0.0f;
				auto u = mob.wantsToJump;
				auto d = moveDown;
				auto r = mob.horizontalMoveDir > 0.0f;
				
				if (l || u || d || r)
				{
					mob.shootDirection.left = l;
					mob.shootDirection.up = u;
					mob.shootDirection.right = r;
					mob.shootDirection.down = d;
				}

				mob.wantsToShoot = false;
				mob.wantsToJump = false;
				mob.horizontalMoveDir = 0.0f;
			}
			else if (Input::IsKeyUp(shootKey))
			{
				mob.horizontalMoveDir = 0.0f;
				mob.wantsToShoot = true;
				mob.readyToShoot = false;
				mob.wantsToJump = false;
			}

			bitBuffers[std::to_string(mob.mobID)] = Utils::ToJSON(mob.CreateEventBitBuffer());
		}

		if (gameCore->frameCounter % 2 == 0)
		{
			std::vector<json> events;
			std::vector<EventType> eventTypes;

			events.push_back(Utils::ToJSON(MobInputsEvent{ bitBuffers }));
			eventTypes.push_back(EventType::MobInputs);

			gameCore->host->SendPacket(PacketData{ events, eventTypes }, ENET_PACKET_FLAG_RELIABLE, 0);
		}
	}

	void ECSSpriteBodyUpdateSystem(GameCore* gameCore, float deltaTime)
	{
		auto& registry = gameCore->registry;
		auto map = gameCore->map;

		for (auto entity : registry.view<PhysicsBodyComponent, SpriteComponent>())
		{
			auto& body = registry.get<PhysicsBodyComponent>(entity).body;
			auto& sprite = registry.get<SpriteComponent>(entity);

			auto drawPos = Vec2fFromB2(body->GetPosition()) * Game::METERS_TO_PIXELS + sprite.pos + sprite.size;
			auto width = map->layers[0]->width * (float)map->layers[0]->tileset->tileWidth * map->mapScale;
			auto height = map->layers[0]->height * (float)map->layers[0]->tileset->tileHeight * map->mapScale;

			if (drawPos.x + sprite.size.x / 2 < 0 && body->GetLinearVelocity().x < 0.0f)
			{
				auto pos = body->GetPosition();
				pos.x = (width + sprite.size.x / 2) * Game::PIXELS_TO_METERS;
				body->SetTransform(pos, body->GetAngle());
			}
			else if (drawPos.x - sprite.size.x / 2 > width && body->GetLinearVelocity().x > 0.0f)
			{
				auto pos = body->GetPosition();
				pos.x = -sprite.size.x / 2 * Game::PIXELS_TO_METERS;
				body->SetTransform(pos, body->GetAngle());
			}
			else if (drawPos.y + sprite.size.y / 2 < 0 && body->GetLinearVelocity().y < 0.0f)
			{
				auto pos = body->GetPosition();
				pos.y = (height + sprite.size.y / 2) * Game::PIXELS_TO_METERS;
				body->SetTransform(pos, body->GetAngle());
			}
			else if (drawPos.y - sprite.size.y / 2 > height && body->GetLinearVelocity().y > 0.0f)
			{
				auto pos = body->GetPosition();
				pos.y = -sprite.size.y / 2 * Game::PIXELS_TO_METERS;
				body->SetTransform(pos, body->GetAngle());
			}
		}
	}
}