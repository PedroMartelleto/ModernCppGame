#include "EventHandler.h"
#include "../GameData.h"
#include "../Spawner.h"

namespace EventHandler
{
	void HandleEvents(GameCore* gameCore)
	{
		auto& queue = gameCore->host->eventQueue;
		auto& registry = gameCore->registry;

		while (!queue.IsEmpty())
		{
			auto event = queue.Dequeue();
			switch (event.type)
			{
				case EventType::MobInputs:
				{
					auto ev = std::static_pointer_cast<MobInputsEvent>(event.data);
					for (auto& entity : registry.view<MobComponent>())
					{
						auto& mob = registry.get<MobComponent>(entity);
						if (ev->bitBuffers.find(std::to_string(mob.mobID)) != ev->bitBuffers.end())
						{
							auto bitBuffer = Utils::FromJSON<BitBuffer8>(ev->bitBuffers[std::to_string(mob.mobID)]);
							mob.wantsToJump = bitBuffer.GetBit(GameData::GetMobActionBit("MOVE_UP"));
							mob.horizontalMoveDir = 0.0f;
							
							if (bitBuffer.GetBit(GameData::GetMobActionBit("MOVE_LEFT")))
							{
								mob.horizontalMoveDir -= 1.0f;
							}

							if (bitBuffer.GetBit(GameData::GetMobActionBit("MOVE_RIGHT")))
							{
								mob.horizontalMoveDir += 1.0f;
							}

							if (bitBuffer.GetBit(GameData::GetMobActionBit("SHOOT_DOWN")))
							{
								mob.readyToShoot = true;
								mob.wantsToShoot = false;
							}

							if (bitBuffer.GetBit(GameData::GetMobActionBit("SHOOT_UP")))
							{
								mob.wantsToShoot = true;
								mob.readyToShoot = false;
							}
						}
					}
				}
				break;
				case EventType::ResetMap:
				{
					// Removes projectiles
					for (auto entity : registry.view<ProjectileComponent>())
					{
						auto* body = registry.try_get<PhysicsBodyComponent>(entity);

						if (body != nullptr)
						{
							gameCore->physicsWorld.DestroyBody(body->body);
						}

						registry.destroy(entity);
					}

					auto nextSpawns = gameCore->map->GetSpawns(4);

					// Resets the health of the players and removes all other mobs from the map.
					for (auto entity : registry.view<MobComponent>())
					{
						auto& mob = registry.get<MobComponent>(entity);

						if (mob.IsPlayer())
						{
							mob.Reset(registry.try_get<ProjectileInventoryComponent>(entity));

							auto* body = registry.try_get<PhysicsBodyComponent>(entity);
							if (body != nullptr)
							{
								body->body->SetTransform(Vec2fToB2(nextSpawns.back() * Game::PIXELS_TO_METERS), 0.0f);
								body->body->SetLinearVelocity(b2Vec2(0, 0));
								body->body->SetAngularVelocity(0);
							}
							nextSpawns.pop_back();
						}
					}
				}
				break;
				case EventType::WorldSnapshot:
				{
					auto ev = std::static_pointer_cast<WorldSnapshotEvent>(event.data);
					gameCore->clientPrediction->RegisterSnapshot(ev);
				}
				break;
				case EventType::Map:
				{
					auto ev = std::static_pointer_cast<MapDataEvent>(event.data);
					gameCore->map = CreateRef<TileMap>(gameCore->physicsWorld, Game::MAP_SCALE, ev->mapXMLContents, gameCore->resourceManager);
					gameCore->core->SetWindowSizeAndCenter((int)gameCore->map->WidthInPixels(), (int)gameCore->map->HeightInPixels());
				}
				break;
				case EventType::SpawnPlayers:
				{
					auto ev = std::static_pointer_cast<SpawnPlayersEvent>(event.data);

					// If no positions were specified, randomly pick spawn positions from the map
					if (ev->positions.size() <= 0)
					{
						for (const auto& pos : gameCore->map->GetSpawns(ev->mobIDs.size()))
						{
							ev->positions.push_back({ pos.x, pos.y });
						}
					}

					for (int i = 0; i < (int)ev->mobIDs.size(); ++i)
					{
						bool isLocal = ev->types[i] == gameCore->host->type;
						Spawner::SpawnPlayer(gameCore, ev->mobIDs[i], ev->charNames[i],
							Vec2f(ev->positions[i].first, ev->positions[i].second), isLocal);
					}
				}
				break;
				case EventType::RemoveMobs:
				{
					auto ev = std::static_pointer_cast<RemoveMobsEvent>(event.data);

					for (auto mobID : ev->mobIDs)
					{
						// Removes entity from registry, removes its body and from the entity map
						if (gameCore->mobs.find(mobID) != gameCore->mobs.end())
						{
							auto entity = gameCore->mobs[mobID];

							auto* body = registry.try_get<PhysicsBodyComponent>(entity);
							if (body != nullptr)
							{
								gameCore->physicsWorld.DestroyBody(body->body);
							}

							gameCore->registry.destroy(entity);
							gameCore->mobs.erase(mobID);
						}
					}
				}
				break;
			}
		}
	}
}