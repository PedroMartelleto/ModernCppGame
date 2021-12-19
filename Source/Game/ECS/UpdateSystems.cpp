#include "UpdateSystems.h"
#include "../GameCore.h"
#include "../GameData.h"
#include "../Spawner.h"
#include "../../Engine/Pathfinding/AStarSearch.h"

namespace UpdateSystems
{
	#define MAX_PLAYERS 4
	std::vector<entt::entity> nearProjectiles[MAX_PLAYERS] = { {} };
	Job* nearProjectilesJobs[MAX_PLAYERS] = { nullptr };

	int nearProjectilesRate = 4;

	void StandbyProjectileUpdateSystem(GameCore* gameCore, float deltaTime)
	{
		auto& registry = gameCore->registry;
		const auto& map = gameCore->map;

		// Every N frames, check if there are projectiles near the player
		if (gameCore->frameCounter % nearProjectilesRate == 0)
		{
			for (uint32_t i = 0; i < gameCore->players.size(); ++i)
			{
				auto playerHandle = gameCore->players[i];

				// If a job was found...
				if (nearProjectilesJobs[i] != nullptr)
				{
					// Waits for its completion and frees memory.
					nearProjectilesJobs[i]->Wait();
					delete nearProjectilesJobs[i];
					nearProjectilesJobs[i] = nullptr;

					// Gets the result (at this point, we can be sure that there is no other thread accessing nearProjectiles).
					for (auto projHandle : nearProjectiles[i])
					{
						// Picks up projectile
 						auto& inventory = registry.get<ProjectileInventoryComponent>(playerHandle);
						auto& projectile = registry.get<ProjectileComponent>(projHandle);
						inventory.projectiles.push_back(projectile.projectileData);
						registry.destroy(projHandle);
					}

					nearProjectiles[i].clear();
				}

				// Gets projectiles to test if they are near the player
				std::vector<std::pair<Vec2f, entt::entity>> standbyProjectiles;
				Vec2f targetPos = Vec2fFromB2(registry.get<PhysicsBodyComponent>(playerHandle).body->GetPosition()) * Game::METERS_TO_PIXELS;

				for (auto projHandle : registry.view<ProjectileComponent, SpriteComponent>())
				{
					auto& projectile = registry.get<ProjectileComponent>(projHandle);
					auto& sprite = registry.get<SpriteComponent>(projHandle);

					if (projectile.hasHitAnything && projectile.projectileData.owner == playerHandle)
					{
						standbyProjectiles.push_back({ sprite.pos, projectile.entityHandle });
					}
				}

				static const float pickupDistance = 1.7f * map->scaledTileSize.x;

				// Schedules a new job
				nearProjectilesJobs[i] = (new Job([i, standbyProjectiles, targetPos]()
				{
					for (const auto& nearProjectile : standbyProjectiles)
					{
						if (Math::DistanceSquared(nearProjectile.first, targetPos) < pickupDistance * pickupDistance)
						{
							nearProjectiles[i].push_back(nearProjectile.second);
						}
					}
				}))->Schedule();
			}
		}
	}

	#define MAX_MOB_ID 4096

	// Dictionaries that map mob id to pathfinding data
	// We need to use arrays since they are thread-safe (as long each thread accesses different indices).
	std::pair<WorldNodeID, WorldNodeID> shortestPaths[MAX_MOB_ID] = { {0, 0} };
	Job* pathfindingJobs[MAX_MOB_ID] = { nullptr };

	int pathfindingRate = 20;

	// Enemy spawning
	float spawnPeriodicityMin = 3.5f;
	float spawnPeriodicityMax = 8.5f;
	int maxEnemies = 12;
	int nextSpawnTicks = 0;

	void ECSMobPathfindingUpdateSystem(GameCore* gameCore, float deltaTime)
	{
		auto& registry = gameCore->registry;
		const auto& map = gameCore->map;

		if (nextSpawnTicks <= 0) // If we can spawn...
		{
			// Define the number of ticks until the next spawn
			nextSpawnTicks = (int)(Utils::FastRandomFloat(spawnPeriodicityMax, spawnPeriodicityMax) * 60);

			std::vector<std::string> possibleMobsToSpawn = { "big_demon", "chort" };
			auto mobToSpawn = possibleMobsToSpawn[Utils::RandomInt(0, possibleMobsToSpawn.size() - 1)];

			if (Utils::FastRandomFloat(0, 1) < 0.1f)
			{
				mobToSpawn = "crazy_demon";
			}

			// Picks a random player to be the target mob
			auto targetMob = registry.get<MobComponent>(gameCore->players[Utils::RandomInt(0, gameCore->players.size()-1)]).mobID;
			auto enemy = Spawner::SpawnEnemyMob(gameCore, gameCore->CreateMobID(), mobToSpawn, gameCore->map->GetSpawn());
			registry.emplace<PathfindingComponent>(enemy, PathfindingComponent{ 0, 0, targetMob });
		}
		else if (gameCore->GetNonPlayerCount() < maxEnemies) // If we can spawn a new mob, decrement tick by one
		{
			nextSpawnTicks -= 1;
		}

		for (auto entity : registry.view<MobComponent, PhysicsBodyComponent, PathfindingComponent>())
		{
			auto& mob = registry.get<MobComponent>(entity);
			auto& pathfinding = registry.get<PathfindingComponent>(entity);
			auto* body = registry.get<PhysicsBodyComponent>(entity).body;

			auto& target = registry.get<PhysicsBodyComponent>(gameCore->mobs[pathfinding.targetMob]);

			auto mobPos = Vec2fFromB2((Game::METERS_TO_PIXELS / map->mapScale) * body->GetPosition());
			auto targetPos = Vec2fFromB2((Game::METERS_TO_PIXELS / map->mapScale) * target.body->GetPosition());

			if (!mob.randomWalk)
			{
				// Follows target directly
				if (mobPos.x < targetPos.x - 3.0f)
				{
					mob.horizontalMoveDir = 1.0f;
				}
				else if (mobPos.x > targetPos.x + 3.0f)
				{
					mob.horizontalMoveDir = -1.0f;
				}
			}

			// Every N frames, perform pathfinding
			if (gameCore->frameCounter % pathfindingRate == 0)
			{
				auto mobID = mob.mobID;
				auto* graph = map->pathfindingGraph.get();

				// If a pathfinding job was found...
				if (pathfindingJobs[mobID] != nullptr)
				{
					// Waits for its completion and frees memory.
					pathfindingJobs[mobID]->Wait();
					delete pathfindingJobs[mobID];
					pathfindingJobs[mobID] = nullptr;
					
					// Gets the result (at this point, we can be sure that there is no other thread accessing shortestPaths[mobID]).
					pathfinding.currentNode = shortestPaths[mobID].first;
					pathfinding.destNode = shortestPaths[mobID].second;
				}

				auto randomWalk = mob.randomWalk;
				
				if (randomWalk)
				{
					mob.wantsToJump = true;
				}

				auto nextNode = pathfinding.destNode;

				// Schedules a new job
				pathfindingJobs[mobID] = (new Job([graph, mobID, mobPos, targetPos, nextNode, randomWalk]()
				{
					auto srcID = graph->GetClosestNode(mobPos);
					if (!randomWalk)
					{
						auto targetNodeID = graph->GetClosestNode(targetPos);
						auto path = AStarSearch::FindShortestPath(*graph, srcID, targetNodeID, [](const Vec2f& a, const Vec2f& b) { return 0.0f; });
						shortestPaths[mobID] = { srcID, path[srcID] };
					}
					else
					{
						shortestPaths[mobID].first = srcID;
						if (srcID == nextNode || srcID == 0 || nextNode == 0)
						{
							const auto& links = graph->at(srcID).links;
							shortestPaths[mobID].second = links[Utils::RandomInt(0, links.size() - 1)].dst;
						}
					}
				}))->Schedule();
			}

			if (pathfinding.currentNode == 0 || pathfinding.destNode == 0) continue;

			const auto& srcNode = map->pathfindingGraph->at(pathfinding.currentNode);
			const auto& nextNode = map->pathfindingGraph->at(pathfinding.destNode);

			// Follows next node in path (takes precedent over following target directly)
			if (srcNode.isHorizontalEdge && nextNode.isHorizontalEdge)
			{
				// Handles left/right edge nodes
				mob.horizontalMoveDir = srcNode.worldPos.x > map->UnscaledWidthInPixels() / 2.0f ? 1.0f : -1.0f;
			}
			else
			{
				if (srcNode.worldPos.x < nextNode.worldPos.x)
				{
					mob.horizontalMoveDir = 1.0f;
				}
				else
				{
					mob.horizontalMoveDir = -1.0f;
				}
			}

			if (srcNode.isBottomEdge)
			{
				mob.horizontalMoveDir = 0.0f;
			}
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
		// Mob movement, winner detection and removal of dead mobs
		for (auto entity : registry.view<PhysicsBodyComponent, MobComponent>())
		{
			auto& body = registry.get<PhysicsBodyComponent>(entity);
			auto& mob = registry.get<MobComponent>(entity);

			if (mob.health <= 0.0f)
			{
				if (mob.IsPlayer())
				{
					gameCore->SetWinner(mob.playerIndex == 0 ? 2 : 1);
				}
				else
				{
					// Safely removes this mob next update (we delay the removal to avoid logic errors and box 2d conflicts).
					gameCore->host->eventQueue.Enqueue(EventType::RemoveMobs, CreateRef<RemoveMobsEvent>(RemoveMobsEvent{ {mob.mobID} }));
				}
			}

			if (mob.invencibilityTicks > 0)
			{
				mob.invencibilityTicks -= 1;
			}

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

			if (mob.wantsToJump || (mob.jumpStartTimestamp > 0 && mob.jumpStartTimestamp < 0.05))
			{
				auto* sensorComponent = registry.try_get<SensorComponent>(entity);

				if (sensorComponent != nullptr && sensorComponent->IsColliding())
				{
					auto force = b2Vec2(0.0f, -mob.jumpHeight * body.body->GetMass() * 50.0f);
					body.body->ApplyForceToCenter(force, true);
					mob.jumpStartTimestamp = Timer::GetTime();
				}
			}


			if (mob.jumpStartTimestamp > 0 && Timer::GetTime() - mob.jumpStartTimestamp >= 0.4)
			{
				auto force = b2Vec2(0.0f, gameCore->physicsWorld.GetGravity().y * 0.85f);

				if (Timer::GetTime() - mob.jumpStartTimestamp >= 0.65)
				{
					force.y *= 1.3f;
				}

				body.body->ApplyForceToCenter(force, true);
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

	void Cleanup()
	{
		// Cleans up any jobs that were not done yet.

		for (int i = 0; i < MAX_MOB_ID; ++i)
		{
			if (pathfindingJobs[i] != nullptr)
			{
				pathfindingJobs[i]->Wait();
				delete pathfindingJobs[i];
				pathfindingJobs[i] = nullptr;
			}
		}

		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			if (nearProjectilesJobs[i] != nullptr)
			{
				nearProjectilesJobs[i]->Wait();
				delete nearProjectilesJobs[i];
				nearProjectilesJobs[i] = nullptr;
			}
		}
	}
}