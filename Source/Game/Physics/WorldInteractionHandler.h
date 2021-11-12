#pragma once

#include "../ECS/Components.h"

enum class InteractionFlag : uint8_t
{
	BEGIN = 0,
	END = 1
};

namespace WorldInteractionHandler
{
	void OnMobProjectileInteraction(InteractionFlag flag, MobComponent* mobA, ProjectileComponent* projectileB);
	void OnMobMobInteraction(InteractionFlag flag, MobComponent* mobA, MobComponent* mobB);
};

