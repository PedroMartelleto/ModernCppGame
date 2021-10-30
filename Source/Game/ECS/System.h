#pragma once

#include "Components.h"

class GameCore;

struct UpdateSystem
{
	UpdateSystem() {}
	virtual void Update(GameCore* gameCore, float deltaTime) = 0;
};

struct RenderSystem
{
	RenderSystem() {}
	virtual void Render(GameCore* gameCore) = 0;
};