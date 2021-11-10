#pragma once

#include <functional>

class GameCore;

using RenderSystemType = std::function<void(GameCore*)>;

namespace RenderSystems
{
	void ECSSpriteRenderSystem(GameCore* gameCore);
	void ECSProjectileInventoryRenderSystem(GameCore* gameCore);

	const std::vector<RenderSystemType> renderSystems = {
		ECSSpriteRenderSystem, ECSProjectileInventoryRenderSystem
	};
};
