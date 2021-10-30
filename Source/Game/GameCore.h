#pragma once

#include "../Engine/Engine.h"
#include "ECS/System.h"

struct RenderSystem;
struct UpdateSystem;

class GameCore
{
public:
	GameCore(Core* core);

	void Create();
	void Update(float deltaTime);
	void Render();
	void Destroy();

	b2Body* CreateDynamicBoxBody(const Vec2f& position, const Vec2f& size, const Vec2f& footRatio, GroundDetectionComponent* groundDetectionComponent);

	void AddRenderSystem(RenderSystem* system);
	void AddUpdateSystem(UpdateSystem* system);
public:
	Core* core;
	TextureManager* textureManager;
	TextureAtlas* atlas;
	TileMap* map;
	b2World physicsWorld;
	entt::registry registry;
private:
	Array<RenderSystem*> m_renderSystems;
	Array<UpdateSystem*> m_updateSystems;
};

