#include "MobBodyUpdateSystem.h"

void MobBodyUpdateSystem::Update(GameCore* gameCore, float deltaTime)
{
	auto& registry = gameCore->registry;

	// Mob movement
	for (auto entity : registry.view<PhysicsBodyComponent, MobComponent>())
	{
		auto& body = registry.get<PhysicsBodyComponent>(entity);
		auto& mob = registry.get<MobComponent>(entity);

		if (mob.wantsToJump)
		{
			auto* groundDetection = registry.try_get<GroundDetectionComponent>(entity);

			if (groundDetection != nullptr && groundDetection->isGrounded)
			{
				auto impulse = Vec2f(0.0f, -mob.jumpHeight * body.body->GetMass()).b2();
				body.body->ApplyLinearImpulse(impulse, b2Vec2(0, 0), true);
			}
		}

		// Movement impulse
		if (mob.horizontalMoveDir != 0.0f)
		{
			auto impulse = Vec2f(mob.horizontalMoveDir * mob.horizontalImpulse, 0.0f).b2();
			body.body->ApplyLinearImpulse(impulse, b2Vec2(0, 0), true);
		}

		// Air drag
		float Cd = 1.05f;
		float v = body.body->GetLinearVelocity().x;
		if (fabsf(v) > 0.0001f)
		{
			auto appliedDrag = Vec2f(-0.5f * v * Cd * mob.horizontalDragForce, 0);
			body.body->ApplyForceToCenter(appliedDrag.b2(), true);

			// Velocity clamp
			auto velocity = body.body->GetLinearVelocity();
			velocity.x = Math::Clamp(velocity.x, -mob.maxHorizontalSpeed, mob.maxHorizontalSpeed);
			body.body->SetLinearVelocity(velocity);
		}
	}
}
