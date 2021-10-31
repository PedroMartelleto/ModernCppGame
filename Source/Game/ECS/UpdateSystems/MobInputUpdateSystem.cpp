#include "MobInputUpdateSystem.h"

void MobInputUpdateSystem::Update(GameCore* gameCore, float deltaTime)
{
	auto& registry = gameCore->registry;

	// Movevement of controllable entities
	for (auto entity : registry.view<PlayerInputComponent, MobComponent>())
	{
		auto& input = registry.get<PlayerInputComponent>(entity);
		auto& mob = registry.get<MobComponent>(entity);
		
		mob.wantsToJump = Input::IsKeyDown(input.JUMP);
		mob.horizontalMoveDir = 0.0f;

		if (Input::IsKeyDown(input.MOVE_LEFT))
		{
			mob.horizontalMoveDir -= 1.0f;
		}

		if (Input::IsKeyDown(input.MOVE_RIGHT))
		{
			mob.horizontalMoveDir += 1.0f;
		}
	}
}
