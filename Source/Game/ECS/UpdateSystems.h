#pragma once

#include <functional>

class GameCore;

using UpdateSystemType = std::function<void(GameCore*, float)>;

namespace UpdateSystems
{
	void ECSMobPathfindingUpdateSystem(GameCore* gameCore, float deltaTime);
	void ECSMobTextureRegionUpdateSystem(GameCore* gameCore, float deltaTime);
	void ECSBodyUpdateSystem(GameCore* gameCore, float deltaTime);
	void ECSMobBodyUpdateSystem(GameCore* gameCore, float deltaTime);
	void ECSMobInputUpdateSystem(GameCore* gameCore, float deltaTime);
	void ECSSpriteBodyUpdateSystem(GameCore* gameCore, float deltaTime);

	const std::vector<UpdateSystemType> updateSystems = {
		ECSMobPathfindingUpdateSystem, ECSMobTextureRegionUpdateSystem, ECSBodyUpdateSystem, ECSMobBodyUpdateSystem, ECSMobInputUpdateSystem, ECSSpriteBodyUpdateSystem
	};
};

