#pragma once

#include "../Engine/Engine.h"
#include "ECS/System.h"
#include "Globals.h"
#include "Networking/NetworkHost.h"

struct RenderSystem;
struct UpdateSystem;

class GameCore
{
public:
	GameCore(Core* core, HostType hostType);

	void Create();
	void Update(float deltaTime);
	void Render();
	void Destroy();
private:
	void SpawnPlayer(const std::string& charName, const Vec2f& tilePos, bool isLocal);

	b2Body* CreateDynamicBoxBody(const Vec2f& position, const Vec2f& size, const Vec2f& footRatio, GroundDetectionComponent* groundDetectionComponent);

	void AddRenderSystem(Ref<RenderSystem> system);
	void AddUpdateSystem(Ref<UpdateSystem> system);
public:
	Core* core;
	Ref<NetworkHost> host;
	Ref<TextureManager> textureManager;
	Ref<TextureAtlas> atlas;
	Ref<TileMap> map;
	b2World physicsWorld;
	entt::registry registry;
private:
	Array<Ref<RenderSystem>> m_renderSystems;
	Array<Ref<UpdateSystem>> m_updateSystems;
	HostType m_hostType;
};

