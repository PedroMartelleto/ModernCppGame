#include "UpdateSystems.h"
#include "../GameCore.h"
#include "../GameData.h"

namespace UpdateSystems
{
	void ECSMobBodyUpdateSystem(GameCore* gameCore, float deltaTime)
	{
		auto& registry = gameCore->registry;

		// Mob movement
		for (auto entity : registry.view<PhysicsBodyComponent, MobComponent>())
		{
			auto& body = registry.get<PhysicsBodyComponent>(entity);
			auto& mob = registry.get<MobComponent>(entity);

			if (mob.wantsToJump)
			{
				auto* sensorComponent = registry.try_get<SensorComponent>(entity);

				if (sensorComponent != nullptr && sensorComponent->IsGrounded())
				{
					auto impulse = b2Vec2(0.0f, -mob.jumpHeight * body.body->GetMass());
					body.body->ApplyLinearImpulse(impulse, b2Vec2(0, 0), true);
				}
			}

			// Movement impulse
			if (mob.horizontalMoveDir != 0.0f)
			{
				auto impulse = b2Vec2(mob.horizontalMoveDir * mob.horizontalImpulse, 0.0f);
				body.body->ApplyLinearImpulse(impulse, b2Vec2(0, 0), true);
			}

			// Air drag
			float Cd = 1.05f;
			float v = body.body->GetLinearVelocity().x;

			if (fabsf(v) > 0.0001f)
			{
				auto appliedDrag = b2Vec2(-0.5f * v * Cd * mob.horizontalDragForce, 0);
				body.body->ApplyForceToCenter(appliedDrag, true);

				// Velocity clamp
				auto velocity = body.body->GetLinearVelocity();
				velocity.x = Math::Clamp(velocity.x, -mob.maxHorizontalSpeed, mob.maxHorizontalSpeed);
				body.body->SetLinearVelocity(velocity);
			}
		}
	}

	void ECSMobInputUpdateSystem(GameCore* gameCore, float deltaTime)
	{
		auto& registry = gameCore->registry;
		json bitBuffers;

		// Movevement of controllable entities
		for (auto entity : registry.view<LocalInputComponent, MobComponent>())
		{
			auto& input = registry.get<LocalInputComponent>(entity);
			auto& mob = registry.get<MobComponent>(entity);

			mob.wantsToJump = Input::IsKeyDown(input.inputCodes[GameData::GetMobActionBit("JUMP")]);
			mob.horizontalMoveDir = 0.0f;

			if (Input::IsKeyDown(input.inputCodes[GameData::GetMobActionBit("MOVE_LEFT")]))
			{
				mob.horizontalMoveDir -= 1.0f;
			}

			if (Input::IsKeyDown(input.inputCodes[GameData::GetMobActionBit("MOVE_RIGHT")]))
			{
				mob.horizontalMoveDir += 1.0f;
			}

			auto left = input.inputCodes[GameData::GetMobActionBit("SHOOT_LEFT")];
			auto up = input.inputCodes[GameData::GetMobActionBit("SHOOT_UP")];
			auto right = input.inputCodes[GameData::GetMobActionBit("SHOOT_RIGHT")];
			auto down = input.inputCodes[GameData::GetMobActionBit("SHOOT_DOWN")];
			mob.shootDirection.left = Input::IsKeyDown(left);
			mob.shootDirection.up = Input::IsKeyDown(up);
			mob.shootDirection.right = Input::IsKeyDown(right);
			mob.shootDirection.down = Input::IsKeyDown(down);

			bitBuffers[std::to_string(mob.mobID)] = Utils::ToJSON(mob.CreateEventBitBuffer());
		}

		if (gameCore->frameCounter % 2 == 0)
		{
			std::vector<json> events;
			std::vector<EventType> eventTypes;

			events.push_back(Utils::ToJSON(MobInputsEvent{ bitBuffers }));
			eventTypes.push_back(EventType::MobInputs);

			gameCore->host->SendPacket(PacketData{ events, eventTypes }, ENET_PACKET_FLAG_RELIABLE, 0);
		}
	}

	void ECSSpriteBodyUpdateSystem(GameCore* gameCore, float deltaTime)
	{
		auto& registry = gameCore->registry;
		auto map = gameCore->map;

		for (auto entity : registry.view<PhysicsBodyComponent, SpriteComponent>())
		{
			auto& body = registry.get<PhysicsBodyComponent>(entity).body;
			auto& sprite = registry.get<SpriteComponent>(entity);

			auto drawPos = Vec2fFromB2(body->GetPosition()) * Game::METERS_TO_PIXELS + sprite.pos + sprite.size;
			auto width = map->layers[0]->width * (float)map->layers[0]->tileset->tileWidth * map->mapScale;
			auto height = map->layers[0]->height * (float)map->layers[0]->tileset->tileHeight * map->mapScale;

			if (drawPos.x + sprite.size.x / 2 < 0 && body->GetLinearVelocity().x < 0.0f)
			{
				auto pos = body->GetPosition();
				pos.x = (width + sprite.size.x / 2) * Game::PIXELS_TO_METERS;
				body->SetTransform(pos, body->GetAngle());
			}
			else if (drawPos.x - sprite.size.x / 2 > width && body->GetLinearVelocity().x > 0.0f)
			{
				auto pos = body->GetPosition();
				pos.x = -sprite.size.x / 2 * Game::PIXELS_TO_METERS;
				body->SetTransform(pos, body->GetAngle());
			}
			else if (drawPos.y + sprite.size.y / 2 < 0 && body->GetLinearVelocity().y < 0.0f)
			{
				auto pos = body->GetPosition();
				pos.y = (height + sprite.size.y / 2) * Game::PIXELS_TO_METERS;
				body->SetTransform(pos, body->GetAngle());
			}
			else if (drawPos.y - sprite.size.y / 2 > height && body->GetLinearVelocity().y > 0.0f)
			{
				auto pos = body->GetPosition();
				pos.y = -sprite.size.y / 2 * Game::PIXELS_TO_METERS;
				body->SetTransform(pos, body->GetAngle());
			}
		}
	}
}