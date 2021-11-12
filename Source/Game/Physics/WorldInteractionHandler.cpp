#include "WorldInteractionHandler.h"

namespace WorldInteractionHandler
{
	void OnMobProjectileInteraction(InteractionFlag flag, MobComponent* mobA, ProjectileComponent* projectileB)
	{
		if (!projectileB->hasHitAnything)
		{
			mobA->health -= projectileB->projectileData.damage;
			LOGGER_VAR(mobA->name);
			LOGGER_VAR(mobA->health);
			projectileB->hasHitAnything = true;
		}
	}

	void OnMobMobInteraction(InteractionFlag flag, MobComponent* mobA, MobComponent* mobB)
	{

	}
}