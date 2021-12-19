#pragma once

#include <functional>

class GameCore;

using RenderSystemType = std::function<void(GameCore*)>;

namespace RenderSystems
{
	void ECSHealthRenderSystem(GameCore* gameCore);
	void ECSSpriteRenderSystem(GameCore* gameCore);
	void ECSProjectileInventoryRenderSystem(GameCore* gameCore);

	void DEBUG_ECSDrawBodies(GameCore* gameCore);

	const std::vector<RenderSystemType> renderSystems = {
		ECSSpriteRenderSystem, ECSProjectileInventoryRenderSystem, ECSHealthRenderSystem,
		DEBUG_ECSDrawBodies
	};
};
