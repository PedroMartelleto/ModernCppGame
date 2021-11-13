#include "RenderSystems.h"
#include "../GameCore.h"

namespace RenderSystems
{
	Vec2f GetBodyDrawPos(const SpriteComponent& sprite, const PhysicsBodyComponent* body)
	{
		auto drawPos = sprite.pos;
		if (body != nullptr)
		{
			drawPos += Vec2fFromB2(body->body->GetPosition()) * Game::METERS_TO_PIXELS;
		}
		return drawPos;
	}

	void ECSSpriteRenderSystem(GameCore* gameCore)
	{
		auto& registry = gameCore->registry;

		// Renders sprites
		for (auto entity : registry.view<SpriteComponent>())
		{
			const auto& sprite = registry.get<SpriteComponent>(entity);
			AnimationComponent* region = registry.try_get<AnimationComponent>(entity);

			const auto* body = registry.try_get<PhysicsBodyComponent>(entity);

			auto drawPos = GetBodyDrawPos(sprite, body);
			float rotation = 0.0f;

			if (body != nullptr)
			{
				rotation = body->body->GetAngle();

				if (region->allowFlip)
				{
					if (body->body->GetLinearVelocity().x > 0.001f)
					{
						region->isFlipped = false;
					}
					else if (body->body->GetLinearVelocity().x < -0.001f)
					{
						region->isFlipped = true;
					}
				}
			}

			if (region != nullptr)
			{
				Rect2D drawRegion = region->GetRect();
				drawRegion.width = !region->isFlipped ? fabsf(drawRegion.width) : -fabsf(drawRegion.width);
				Render2D::DrawRect(drawPos, rotation, sprite.size, sprite.zIndex, drawRegion, sprite.texture, sprite.tint);
			}
			else
			{
				Render2D::DrawRect(drawPos, rotation, sprite.size, sprite.zIndex, sprite.texture, sprite.tint);
			}
		}
	}

	void ECSProjectileInventoryRenderSystem(GameCore* gameCore)
	{
		auto& registry = gameCore->registry;

		// Renders projectiles ready to be launched
		for (auto entity : registry.view<MobComponent, PhysicsBodyComponent, SpriteComponent, ProjectileInventoryComponent>())
		{
			const auto& mob = registry.get<MobComponent>(entity);
			const auto& inventory = registry.get<ProjectileInventoryComponent>(entity);

			if (!mob.shootDirection.IsNone() && inventory.projectiles.size() > 0)
			{
				const auto& sprite = registry.get<SpriteComponent>(entity);
				const auto& body = registry.get<PhysicsBodyComponent>(entity);
				const auto& projectile = inventory.projectiles.back();

				auto drawPos = GetBodyDrawPos(sprite, &body);

				auto projectileDir = glm::normalize(mob.shootDirection.AsVector());
				float projectileAngle = mob.shootDirection.AsAngle() + glm::radians(projectile.baseAngle);
				
				Rect2D region = gameCore->GetAtlas(projectile.atlas)->GetRegion(projectile.readySpriteName);
				Vec2f size = region.size() * gameCore->map->mapScale;

				Render2D::DrawRect(drawPos + projectileDir * sprite.size/2.0f + Vec2f(8, 12), projectileAngle, size, sprite.zIndex + 100, region, sprite.texture, sprite.tint);
			}
		}
	}
}