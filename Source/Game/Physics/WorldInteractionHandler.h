#pragma once

#include "../ECS/Components.h"

enum class InteractionFlag : uint8_t
{
	BEGIN = 0,
	END = 1
};

template<typename T>
struct ContactData
{
	T* data;
	b2Fixture* fixture;
};

class WorldInteractionHandler
{
public:
	GameCore* gameCore;

	WorldInteractionHandler(GameCore* gameCore) : gameCore(gameCore) {}

	void OnPostSolveMapProjectile(const ContactData<void>& mapA, const ContactData<ProjectileComponent>& projectileB,
								  b2Contact* contact, const b2ContactImpulse* impulse);
	void OnMobProjectileInteraction(InteractionFlag flag, const ContactData<MobComponent>& mobA, const ContactData<ProjectileComponent>& projectileB, b2Contact* contact);
	void OnMobMobInteraction(InteractionFlag flag, const ContactData<MobComponent>& mobA, const ContactData<MobComponent>& mobB, b2Contact* contact);
};

