#pragma once

#include "../../GameCore.h"

struct SpriteRenderSystem : public RenderSystem
{
	void Render(GameCore* gameCore) override;
};

