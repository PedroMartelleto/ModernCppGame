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

	void ECSHealthRenderSystem(GameCore* gameCore)
	{
		auto& registry = gameCore->registry;
		auto atlas = gameCore->GetAtlas(0);
		auto emptyHeart = atlas->GetRegion("ui_heart_empty");
		auto fullHeart = atlas->GetRegion("ui_heart_full");
		auto halfHeart = atlas->GetRegion("ui_heart_half");
		auto heartSize = fullHeart.size() * 2.0f;
		
		auto uiPos = Vec2f(32, 32);

		// Renders health bars
		for (auto entity : registry.view<MobComponent>())
		{
			auto& mob = registry.get<MobComponent>(entity);

			if (mob.IsPlayer())
			{
				auto portrait = atlas->GetRegion("ui_" + mob.name + "_portrait");
				auto drawY = (float)mob.playerIndex * 42.0f;

				Render2D::DrawRect(uiPos + Vec2f(0, -6 + drawY), 0.0f, portrait.size() * 2.0f, Render2D::MAX_Z - 10, portrait, atlas->texture, Colors::WHITE);

				for (int i = 0; i < mob.maxHealth; ++i)
				{
					auto pos = uiPos + Vec2f(64.0f + i * 32.0f, drawY);
					if (mob.health >= i+1)
					{
						Render2D::DrawRect(pos, 0.0f, heartSize, Render2D::MAX_Z - 10, fullHeart, atlas->texture, Colors::WHITE);
					}
					else if (mob.health >= (float)i + 0.5f)
					{
						Render2D::DrawRect(pos, 0.0f, heartSize, Render2D::MAX_Z - 10, halfHeart, atlas->texture, Colors::WHITE);
					}
					else
					{
						Render2D::DrawRect(pos, 0.0f, heartSize, Render2D::MAX_Z - 10, emptyHeart, atlas->texture, Colors::WHITE);
					}
				}
			}
		}
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
		for (auto entity : registry.view<MobComponent, PhysicsBodyComponent, SpriteComponent, ProjectileInventoryComponent, AnimationComponent>())
		{
			const auto& mob = registry.get<MobComponent>(entity);
			const auto& inventory = registry.get<ProjectileInventoryComponent>(entity);
			const auto& anim = registry.get<AnimationComponent>(entity);

			if (mob.readyToShoot && inventory.projectiles.size() > 0)
			{
				const auto& sprite = registry.get<SpriteComponent>(entity);
				const auto& body = registry.get<PhysicsBodyComponent>(entity);
				const auto& projectile = inventory.projectiles.back();

				auto drawPos = GetBodyDrawPos(sprite, &body);
				auto faceDir = anim.GetFaceDir();

				auto projectileDir = glm::normalize(mob.shootDirection.AsVector(faceDir));
				float projectileAngle = mob.shootDirection.AsAngle(faceDir) + glm::radians(projectile.baseAngle);
				
				Rect2D region = gameCore->GetAtlas(projectile.atlas)->GetRegion(projectile.readySpriteName);
				Vec2f size = region.size() * gameCore->map->mapScale;

				Render2D::DrawRect(drawPos + projectileDir * sprite.size/2.0f + Vec2f(7, 11), projectileAngle, size, sprite.zIndex + 100, region, sprite.texture, sprite.tint);
			}
		}
	}
}