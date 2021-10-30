#pragma once

#include "../../GameCore.h"

struct SpriteBodyUpdateSystem : public UpdateSystem
{
	void Update(GameCore* gameCore, float deltaTime);
};

