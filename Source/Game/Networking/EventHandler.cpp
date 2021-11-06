#include "EventHandler.h"
#include "../GameData.h"

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
				case EventType::MobInputsEvent:
				{
					auto ev = std::static_pointer_cast<MobInputsEvent>(event.data);
					for (auto& entity : registry.view<MobComponent>())
					{
						auto& mob = registry.get<MobComponent>(entity);
						if (ev->bitBuffers.find(std::to_string(mob.mobID)) != ev->bitBuffers.end())
						{
							auto bitBuffer = Utils::FromJSON<BitBuffer8>(ev->bitBuffers[std::to_string(mob.mobID)]);
							mob.wantsToJump = bitBuffer.Get(GameData::GetMobActionBit("JUMP"));
							mob.horizontalMoveDir = 0.0f;

							if (bitBuffer.Get(GameData::GetMobActionBit("MOVE_LEFT")))
							{
								mob.horizontalMoveDir -= 1.0f;
							}

							if (bitBuffer.Get(GameData::GetMobActionBit("MOVE_RIGHT")))
							{
								mob.horizontalMoveDir += 1.0f;
							}
						}
					}
				}
				break;
				case EventType::MobPositionsBuffer:
				{
					auto ev = std::static_pointer_cast<MobPositionsEvent>(event.data);
					for (auto& entity : registry.view<PhysicsBodyComponent, MobComponent>())
					{
						// TODO: figure out how to "sort things out" on the client
						// TODO: only send "input updates", not things every frame
						// TODO: input backtracking, delay the client
						// TODO: std::to_string is a bad solution

						auto* body = registry.get<PhysicsBodyComponent>(entity).body;
						auto& mob = registry.get<MobComponent>(entity);
						auto newPos = Utils::FromJSON<b2Vec2>(ev->pos[std::to_string(mob.mobID)]);
						auto newVel = Utils::FromJSON<b2Vec2>(ev->vel[std::to_string(mob.mobID)]);
						body->SetTransform(newPos, body->GetAngle());
						body->SetLinearVelocity(newVel);
					}
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
					gameCore->SpawnPlayer(ev->mobID, ev->charName, Vec2f((float)ev->tileX, (float)ev->tileY), isLocal);
				}
				break;
			}
		}
	}
}