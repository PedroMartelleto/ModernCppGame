#include "WorldInteractionHandler.h"
#include "../GameCore.h"
#include "../GameData.h"

#define PROJECTILE_STICK_MAP_HARDNESS 4.5f

void WorldInteractionHandler::OnPostSolveMapProjectile(const ContactData<void>& mapA, const ContactData<ProjectileComponent>& projectileB,
													   b2Contact* contact, const b2ContactImpulse* impulse)
{
	auto* sprite = gameCore->registry.try_get<SpriteComponent>(projectileB.data->entityHandle);
	if (sprite != nullptr)
	{
		b2WorldManifold manifold;
		contact->GetWorldManifold(&manifold);

		if (!sprite->transformedFlag)
		{
			b2Vec2 stickPos = projectileB.fixture->GetBody()->GetPosition();

			if (contact->GetManifold()->pointCount > 0 && impulse->normalImpulses[0] > PROJECTILE_STICK_MAP_HARDNESS)
			{
				stickPos = manifold.points[contact->GetManifold()->pointCount - 1];
			}

			sprite->pos += Vec2fFromB2(stickPos) * Game::METERS_TO_PIXELS;
			sprite->angle = projectileB.fixture->GetBody()->GetAngle();
			sprite->transformedFlag = true;
		}
	}

	gameCore->QueueBodyForRemoval(projectileB.data->entityHandle);
	projectileB.data->hasHitAnything = true;
}

void WorldInteractionHandler::OnMobProjectileInteraction(InteractionFlag flag, const ContactData<MobComponent>& mobA,
														 const ContactData<ProjectileComponent>& projectileB, b2Contact* contact)
{
	if (projectileB.fixture->IsSensor() && projectileB.fixture->GetBody()->GetLinearVelocity().LengthSquared() > 1.0f)
	{
		mobA.data->Damage(projectileB.data->projectileData.damage);
	}
}

void HandleMobContact(const ContactData<MobComponent>& mobA, const ContactData<MobComponent>& mobB)
{
	if (mobA.data->contactDamage > 0.0f && mobB.data->invencibilityTicks <= 0)
	{
		// Applies small impulse that moves the mob away
		auto impulse = mobB.fixture->GetBody()->GetPosition() - mobA.fixture->GetBody()->GetPosition();
		impulse.Normalize();
		impulse *= 15.0f;
		impulse.y = -6.5f;
		mobB.fixture->GetBody()->ApplyLinearImpulseToCenter(impulse, true);
		mobB.data->Damage(mobA.data->contactDamage);
	}
}

void WorldInteractionHandler::OnMobMobInteraction(InteractionFlag flag, const ContactData<MobComponent>& mobA,
												  const ContactData<MobComponent>& mobB, b2Contact* contact)
{
	if (flag == InteractionFlag::BEGIN)
	{
		// On contact, reduce the health of the mobs involved.

		// We only care about enemy-player contacts (players and enemies don't damage mobs of the same type)
		if ((mobB.data->playerIndex >= 0) ^ (mobA.data->playerIndex >= 0))
		{
			HandleMobContact(mobA, mobB);
			HandleMobContact(mobB, mobA);
		}
	}
}

void WorldInteractionHandler::OnMobMapInteraction(InteractionFlag flag, const ContactData<MobComponent>& mobA, const ContactData<void>& mapB, b2Contact* contact)
{
	if (mobA.fixture->IsSensor() && Timer::GetTime() - mobA.data->jumpStartTimestamp > 0.1)
	{
		mobA.data->jumpStartTimestamp = 0;
	}
}