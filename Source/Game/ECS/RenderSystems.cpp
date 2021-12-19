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
		for (auto entity : registry.view<MobComponent, ProjectileInventoryComponent>())
		{
			const auto& mob = registry.get<MobComponent>(entity);
			const auto& inventory = registry.get<ProjectileInventoryComponent>(entity);

			if (mob.IsPlayer())
			{
				auto portrait = atlas->GetRegion("ui_" + mob.name + "_portrait");
				auto drawY = (float)mob.playerIndex * 42.0f;

				Render2D::DrawRect(uiPos + Vec2f(0, -6 + drawY), 0.0f, portrait.size() * 2.0f, ZPlanes::UI, portrait, atlas->texture, Colors::WHITE);
				Vec2f drawPos = uiPos + Vec2f(24.0f, drawY);

				for (int i = 0; i < mob.maxHealth; ++i)
				{
					drawPos += Vec2f(32.0f, 0.0f);
					if (mob.health >= i+1)
					{
						Render2D::DrawRect(drawPos, 0.0f, heartSize, ZPlanes::UI - 1.0f, fullHeart, atlas->texture, Colors::WHITE);
					}
					else if (mob.health >= (float)i + 0.5f)
					{
						Render2D::DrawRect(drawPos, 0.0f, heartSize, ZPlanes::UI - 1.0f, halfHeart, atlas->texture, Colors::WHITE);
					}
					else
					{
						Render2D::DrawRect(drawPos, 0.0f, heartSize, ZPlanes::UI - 1.0f, emptyHeart, atlas->texture, Colors::WHITE);
					}
				}

				std::string projectileCountStr = "AMMO: " + std::to_string(inventory.projectiles.size());
				drawPos += Vec2f(38, 6) + Render2D::GetTextSize(1.0f, projectileCountStr, gameCore->defaultFont) / 2.0f;
				Render2D::DrawLineOfText(drawPos, 1.0f, projectileCountStr, ZPlanes::UI + 1.0f, gameCore->defaultFont);
			}
		}
	}

	void ECSSpriteRenderSystem(GameCore* gameCore)
	{
		auto& registry = gameCore->registry;

		// Renders sprites
		for (auto entity : registry.view<SpriteComponent>())
		{
			auto& sprite = registry.get<SpriteComponent>(entity);
			AnimationComponent* region = registry.try_get<AnimationComponent>(entity);

			const auto* body = registry.try_get<PhysicsBodyComponent>(entity);

			auto drawPos = GetBodyDrawPos(sprite, body);
			float rotation = sprite.angle;

			// Deals with flipped and rotated bodies
			if (body != nullptr)
			{
				rotation += body->body->GetAngle();

				if (region->allowFlip)
				{
					if (body->body->GetLinearVelocity().x > 0.1f)
					{
						region->isFlipped = false;
					}
					else if (body->body->GetLinearVelocity().x < -0.1f)
					{
						region->isFlipped = true;
					}
				}
			}

			if (region != nullptr)
			{
				const auto* mob = registry.try_get<MobComponent>(entity);

				// Makes the mob "blink" when hit
				Color4f colorMultiplier = Color4f(1.0f, 1.0f, 1.0f, 1.0f);

				if (mob != nullptr)
				{
					if (mob->randomWalk)
					{
						colorMultiplier = Color4f(1.0f/2.83f, 1.0f/0.34f, 1.0f/0.49f, 1.0f);
					}

					if (mob->invencibilityTicks > 0 && (mob->invencibilityTicks / 8) % 2 == 0)
					{
						colorMultiplier *= 1000.0f;
					}
				}

				// Deals with flipping the mob depending on face direction
				Rect2D drawRegion = region->GetRect();
				drawRegion.width = !region->isFlipped ? fabsf(drawRegion.width) : -fabsf(drawRegion.width);

				Render2D::DrawRect(drawPos, rotation, sprite.size, sprite.zIndex, drawRegion, sprite.texture, sprite.tint * colorMultiplier);
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
			const auto& body = registry.get<PhysicsBodyComponent>(entity);
			const auto& sprite = registry.get<SpriteComponent>(entity);

			auto drawPos = GetBodyDrawPos(sprite, &body);
			
			Render2D::DrawLineOfText(drawPos - Vec2f(sprite.pos.x / 2.0f, 10.0f), 1.0f, "P" + std::to_string(mob.playerIndex+1), ZPlanes::UI, gameCore->defaultFont);

			if (mob.readyToShoot && inventory.projectiles.size() > 0)
			{
				const auto& projectile = inventory.projectiles.back();

				auto faceDir = anim.GetFaceDir();

				auto projectileDir = glm::normalize(mob.shootDirection.AsVector(faceDir));
				float projectileAngle = mob.shootDirection.AsAngle(faceDir) + glm::radians(projectile.baseAngle);
				
				Rect2D region = gameCore->GetAtlas(projectile.atlas)->GetRegion(projectile.readySpriteName);
				Vec2f size = region.size() * gameCore->map->mapScale;

				Render2D::DrawRect(drawPos + projectileDir * sprite.size/2.0f + Vec2f(7, 11), projectileAngle,
								   size, ZPlanes::UI, region, sprite.texture, sprite.tint);
			}
		}
	}

	void DEBUG_ECSDrawBodies(GameCore* gameCore)
	{
		auto& registry = gameCore->registry;

		// Renders projectiles ready to be launched
		for (auto entity : registry.view<DEBUG_PhysicsBodyDrawComponent, PhysicsBodyComponent>())
		{
			auto* body = registry.get<PhysicsBodyComponent>(entity).body;
			auto& debug = registry.get<DEBUG_PhysicsBodyDrawComponent>(entity);

			if (debug.drawPoly)
			{
				b2Fixture* fixture = body->GetFixtureList();
				auto color = Colors::RED;
				auto thickness = 2.0f;
				auto offset = Vec2fFromB2(body->GetPosition()) * Game::METERS_TO_PIXELS;

				while (fixture != NULL)
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)fixture->GetShape();
					for (int i = 1; i < polygonShape->m_count; ++i)
					{
						auto start = Vec2fFromB2(polygonShape->m_vertices[i - 1]) * Game::METERS_TO_PIXELS + offset;
						auto end = Vec2fFromB2(polygonShape->m_vertices[i]) * Game::METERS_TO_PIXELS + offset;
						Render2D::DrawLine(false, start, end, ZPlanes::DEBUG, thickness, color);
					}
					auto start = Vec2fFromB2(polygonShape->m_vertices[polygonShape->m_count - 1]) * Game::METERS_TO_PIXELS + offset;
					auto end = Vec2fFromB2(polygonShape->m_vertices[0]) * Game::METERS_TO_PIXELS + offset;
					Render2D::DrawLine(false, start, end, ZPlanes::DEBUG, thickness, color);
					fixture = fixture->GetNext();
				}
			}
		}
	}
}