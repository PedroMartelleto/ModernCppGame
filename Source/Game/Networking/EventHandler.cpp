#include "EventHandler.h"

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
				case EventType::MobPositionsBuffer:
				{
					auto ev = std::static_pointer_cast<MobPositionsEvent>(event.data);
					for (auto& entity : registry.view<PhysicsBodyComponent, MobComponent>())
					{
						// TODO: std::to_string is a bad solution
						// TODO: Client-side prediction
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