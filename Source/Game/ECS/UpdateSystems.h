#pragma once

#include <functional>

class GameCore;

using UpdateSystemType = std::function<void(GameCore*, float)>;

namespace UpdateSystems
{
	void ECSMobTextureRegionUpdateSystem(GameCore* gameCore, float deltaTime);
	void ECSBodyUpdateSystem(GameCore* gameCore, float deltaTime);
	void ECSMobBodyUpdateSystem(GameCore* gameCore, float deltaTime);
	void ECSMobInputUpdateSystem(GameCore* gameCore, float deltaTime);
	void ECSSpriteBodyUpdateSystem(GameCore* gameCore, float deltaTime);

	const std::vector<UpdateSystemType> updateSystems = {
		ECSMobTextureRegionUpdateSystem, ECSBodyUpdateSystem, ECSMobBodyUpdateSystem, ECSMobInputUpdateSystem, ECSSpriteBodyUpdateSystem
	};
};

