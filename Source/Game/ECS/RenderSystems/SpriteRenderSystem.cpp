#include "SpriteRenderSystem.h"

void SpriteRenderSystem::Render(GameCore* gameCore)
{
	auto& registry = gameCore->registry;

	// NOTE: These should not be done here...
	registry.sort<SpriteComponent>([](const auto& lhs, const auto& rhs) {
		return lhs.zIndex < rhs.zIndex;
		});

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
			drawPos += Vec2f(body->body->GetPosition()) * METERS_TO_PIXELS;
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
			DrawTexturePro(sprite.texture, *region, raylib::CreateRectangle(drawPos + sprite.size, sprite.size), (sprite.size / 2).raylib(), ToDegrees(rotation), sprite.tint);
		}
		else
		{
			DrawTextureEx(sprite.texture, drawPos.raylib(), ToDegrees(rotation), sprite.scale, sprite.tint);
		}
	}
}