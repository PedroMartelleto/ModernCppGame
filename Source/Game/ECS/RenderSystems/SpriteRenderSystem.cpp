#include "SpriteRenderSystem.h"

void SpriteRenderSystem::Render(GameCore* gameCore)
{
	auto& registry = gameCore->registry;

	// Renders sprites
	for (auto entity : registry.view<SpriteComponent>())
	{
		auto& sprite = registry.get<SpriteComponent>(entity);
		auto* region = registry.try_get<TextureRegionComponent>(entity);
		auto* body = registry.try_get<PhysicsBodyComponent>(entity);

		auto drawPos = sprite.pos;
		float rotation = 0.0f;

		if (body != nullptr)
		{
			drawPos += Vec2fFromB2(body->body->GetPosition()) * Game::METERS_TO_PIXELS;
			rotation = body->body->GetAngle();

			if (region != nullptr && region->allowFlip)
			{
				if (body->body->GetLinearVelocity().x > 0.001f)
				{
					region->texSize.x = fabsf(region->texSize.x);
				}
				else if (body->body->GetLinearVelocity().x < -0.001f)
				{
					region->texSize.x = -fabsf(region->texSize.x);
				}
			}
		}

		if (region != nullptr)
		{
			Render2D::DrawRect(drawPos, sprite.size, sprite.zIndex, *region, sprite.texture, sprite.tint);
		}
		else
		{
			Render2D::DrawRect(drawPos, sprite.size, sprite.zIndex, sprite.texture, sprite.tint);
		}
	}
}