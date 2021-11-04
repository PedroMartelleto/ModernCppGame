#pragma once

#include "../Engine/Engine.h"
#include "ECS/System.h"
#include "Globals.h"
#include "Networking/NetworkHost.h"
#include "Networking/EventQueue.h"

struct RenderSystem;
struct UpdateSystem;

class GameCore
{
public:
	GameCore(Core* core, HostType hostType);

	void SpawnPlayer(MobID playerID, const std::string& charName, const Vec2f& tilePos, bool isLocal);

	void Create();
	void Update(float deltaTime);
	void Render();
	void Destroy();

	MobID CreateMobID();
private:
	void SetupServer();

	b2Body* CreateDynamicBoxBody(const Vec2f& position, const Vec2f& size, const Vec2f& footRatio, GroundDetectionComponent* groundDetectionComponent);

	void AddRenderSystem(Ref<RenderSystem> system);
	void AddUpdateSystem(Ref<UpdateSystem> system);
public:
	std::unordered_map<MobID, entt::entity> mobs;
	Core* core;
	Ref<NetworkHost> host = nullptr;
	Ref<TextureManager> textureManager;
	Ref<TextureAtlas> atlas;
	Ref<TileMap> map;
	b2World physicsWorld;
	entt::registry registry;
	const std::string mapFilepath;
	uint64_t m_frameCounter = 0;
	
	MobID m_globalMobID = 0;
private:
	Array<Ref<RenderSystem>> m_renderSystems;
	Array<Ref<UpdateSystem>> m_updateSystems;
	const HostType m_hostType;
};

