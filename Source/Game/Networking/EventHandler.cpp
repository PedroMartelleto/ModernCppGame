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
							mob.wantsToJump = bitBuffer.Get(GameData::GetMobActionBit("MOVE_UP"));
							mob.horizontalMoveDir = 0.0f;
							
							if (bitBuffer.Get(GameData::GetMobActionBit("MOVE_LEFT")))
							{
								mob.horizontalMoveDir -= 1.0f;
							}

							if (bitBuffer.Get(GameData::GetMobActionBit("MOVE_RIGHT")))
							{
								mob.horizontalMoveDir += 1.0f;
							}

							if (bitBuffer.Get(GameData::GetMobActionBit("SHOOT_DOWN")))
							{
								mob.readyToShoot = true;
								mob.wantsToShoot = false;
							}

							if (bitBuffer.Get(GameData::GetMobActionBit("SHOOT_UP")))
							{
								mob.wantsToShoot = true;
								mob.readyToShoot = false;
							}
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
					gameCore->map = CreateRef<TileMap>(gameCore->physicsWorld, Game::MAP_SCALE, ev->mapXMLContents, gameCore->textureManager);
					gameCore->core->SetWindowSizeAndCenter((int)gameCore->map->WidthInPixels(), (int)gameCore->map->HeightInPixels());
				}
				break;
				case EventType::SpawnPlayer:
				{
					auto ev = std::static_pointer_cast<SpawnPlayerEvent>(event.data);
					bool isLocal = ev->type == gameCore->host->type;
					Spawner::SpawnPlayer(gameCore, ev->mobID, ev->charName, Vec2f((float)ev->tileX, (float)ev->tileY), isLocal);
				}
				break;
			}
		}
	}
}