#include "SpriteBodyUpdateSystem.h"

void SpriteBodyUpdateSystem::Update(GameCore* gameCore, float deltaTime)
{
	auto& registry = gameCore->registry;
	auto* map = gameCore->map;

	for (auto entity : registry.view<PhysicsBodyComponent, SpriteComponent>())
	{
		auto& body = registry.get<PhysicsBodyComponent>(entity).body;
		auto& sprite = registry.get<SpriteComponent>(entity);

		auto drawPos = Vec2f(body->GetPosition()) * METERS_TO_PIXELS + sprite.pos + sprite.size;
		auto width = map->layers[0]->GetWidth() * (float)map->layers[0]->GetTileset()->tileWidth * map->mapScale;
		auto height = map->layers[0]->GetHeight() * (float)map->layers[0]->GetTileset()->tileHeight * map->mapScale;

		if (drawPos.x + sprite.size.x / 2 < 0 && body->GetLinearVelocity().x < 0.0f)
		{
			auto pos = body->GetPosition();
			pos.x = (width + sprite.size.x / 2) * PIXELS_TO_METERS;
			body->SetTransform(pos, body->GetAngle());
		}
		else if (drawPos.x - sprite.size.x / 2 > width && body->GetLinearVelocity().x > 0.0f)
		{
			auto pos = body->GetPosition();
			pos.x = -sprite.size.x / 2 * PIXELS_TO_METERS;
			body->SetTransform(pos, body->GetAngle());
		}
		else if (drawPos.y + sprite.size.y / 2 < 0 && body->GetLinearVelocity().y < 0.0f)
		{
			auto pos = body->GetPosition();
			pos.y = (height + sprite.size.y / 2) * PIXELS_TO_METERS;
			body->SetTransform(pos, body->GetAngle());
		}
		else if (drawPos.y - sprite.size.y / 2 > height && body->GetLinearVelocity().y > 0.0f)
		{
			auto pos = body->GetPosition();
			pos.y = -sprite.size.y / 2 * PIXELS_TO_METERS;
			body->SetTransform(pos, body->GetAngle());
		}
	}
}