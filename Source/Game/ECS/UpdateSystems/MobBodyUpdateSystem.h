#pragma once

#include "../../GameCore.h"

struct MobBodyUpdateSystem : public UpdateSystem
{
	void Update(GameCore* gameCore, float deltaTime) override;
};

