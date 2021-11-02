#include "MobInputUpdateSystem.h"
#include "../../GameData.h"

void MobInputUpdateSystem::Update(GameCore* gameCore, float deltaTime)
{
	auto& registry = gameCore->registry;

	// Movevement of controllable entities
	for (auto entity : registry.view<PlayerInputComponent, MobComponent>())
	{
		auto& input = registry.get<PlayerInputComponent>(entity);
		auto& mob = registry.get<MobComponent>(entity);

		mob.wantsToJump = Input::IsKeyDown(input.inputCodes[GameData::GetPlayerActionBit("JUMP")]);
		mob.horizontalMoveDir = 0.0f;

		if (Input::IsKeyDown(input.inputCodes[GameData::GetPlayerActionBit("MOVE_LEFT")]))
		{
			mob.horizontalMoveDir -= 1.0f;
		}

		if (Input::IsKeyDown(input.inputCodes[GameData::GetPlayerActionBit("MOVE_RIGHT")]))
		{
			mob.horizontalMoveDir += 1.0f;
		}

		bool anyInput = mob.wantsToJump || fabsf(mob.horizontalMoveDir) > 0.001f;

		if (anyInput)
		{
			// TODO: Temp code below
			std::string msg = mob.wantsToJump ? "J" : "";
			if (mob.horizontalMoveDir != 0.0f) msg = msg + "M";

			gameCore->host->SendPacket(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE, 0);
		}
	}
}
