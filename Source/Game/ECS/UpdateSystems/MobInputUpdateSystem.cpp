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

		bool anyInput = mob.wantsToJump || fabsf(mob.horizontalMoveDir) > 0.001f;

		if (anyInput)
		{
			gameCore->host->SendPacket(mob.CreateEventPacket(), ENET_PACKET_FLAG_RELIABLE, 0);
		}
	}
}
