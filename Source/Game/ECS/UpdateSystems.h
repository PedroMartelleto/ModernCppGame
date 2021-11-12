#pragma once

#include <functional>

class GameCore;

using UpdateSystemType = std::function<void(GameCore*, float)>;

namespace UpdateSystems
{
	void ECSMobBodyUpdateSystem(GameCore* gameCore, float deltaTime);
	void ECSMobInputUpdateSystem(GameCore* gameCore, float deltaTime);
	void ECSSpriteBodyUpdateSystem(GameCore* gameCore, float deltaTime);

	const std::vector<UpdateSystemType> updateSystems = {
		ECSMobBodyUpdateSystem, ECSMobInputUpdateSystem, ECSSpriteBodyUpdateSystem
	};
};
