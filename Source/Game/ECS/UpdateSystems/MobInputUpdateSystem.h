#pragma once

#include "../../GameCore.h"

struct MobInputUpdateSystem : public UpdateSystem
{
	void Update(GameCore* gameCore, float deltaTime) override;
};

