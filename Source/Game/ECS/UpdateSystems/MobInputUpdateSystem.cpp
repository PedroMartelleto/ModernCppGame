#include "MobInputUpdateSystem.h"
#include "../../GameData.h"

void MobInputUpdateSystem::Update(GameCore* gameCore, float deltaTime)
{
	auto& registry = gameCore->registry;
	json bitBuffers;

	// Movevement of controllable entities
	for (auto entity : registry.view<LocalInputComponent, MobComponent>())
	{
		auto& input = registry.get<LocalInputComponent>(entity);
		auto& mob = registry.get<MobComponent>(entity);

		mob.wantsToJump = Input::IsKeyDown(input.inputCodes[GameData::GetMobActionBit("JUMP")]);
		mob.horizontalMoveDir = 0.0f;

		if (Input::IsKeyDown(input.inputCodes[GameData::GetMobActionBit("MOVE_LEFT")]))
		{
			mob.horizontalMoveDir -= 1.0f;
		}

		if (Input::IsKeyDown(input.inputCodes[GameData::GetMobActionBit("MOVE_RIGHT")]))
		{
			mob.horizontalMoveDir += 1.0f;
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
