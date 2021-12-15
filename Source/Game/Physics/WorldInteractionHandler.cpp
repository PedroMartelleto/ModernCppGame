#include "WorldInteractionHandler.h"
#include "../GameCore.h"
#include "../GameData.h"

#define PROJECTILE_STICK_MAP_HARDNESS 3.6f

void WorldInteractionHandler::OnPostSolveMapProjectile(const ContactData<void>& mapA, const ContactData<ProjectileComponent>& projectileB,
													   b2Contact* contact, const b2ContactImpulse* impulse)
{
	if (impulse->normalImpulses[0] <= PROJECTILE_STICK_MAP_HARDNESS) return;

	b2Vec2 anchor = projectileB.fixture->GetBody()->GetWorldPoint(b2Vec2(
		0, -projectileB.data->projectileData.GetAABB().height / 2.0f * Game::MAP_SCALE * Game::PIXELS_TO_METERS));

	b2WeldJointDef weldJointDef;
	weldJointDef.bodyA = mapA.fixture->GetBody();
	weldJointDef.bodyB = projectileB.fixture->GetBody();
	weldJointDef.localAnchorA = weldJointDef.bodyA->GetLocalPoint(anchor);
	weldJointDef.localAnchorB = weldJointDef.bodyB->GetLocalPoint(anchor);
	weldJointDef.referenceAngle = weldJointDef.bodyB->GetAngle() - weldJointDef.bodyA->GetAngle();
	gameCore->AddJointWhenPossible(weldJointDef);

	projectileB.fixture->GetBody()->SetAwake(false);

	projectileB.data->hasHitAnything = true;
}

void WorldInteractionHandler::OnMobProjectileInteraction(InteractionFlag flag, const ContactData<MobComponent>& mobA,
														 const ContactData<ProjectileComponent>& projectileB, b2Contact* contact)
{
	if (!projectileB.data->hasHitAnything && projectileB.fixture->IsSensor() && projectileB.fixture->GetBody()->GetLinearVelocity().LengthSquared() > 1.0f)
	{
		mobA.data->Damage(projectileB.data->projectileData.damage);
		projectileB.data->hasHitAnything = true;
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