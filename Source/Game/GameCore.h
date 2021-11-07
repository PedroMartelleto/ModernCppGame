#pragma once

#include "../Engine/Engine.h"
#include "ECS/System.h"
#include "Globals.h"
#include "Networking/NetworkHost.h"
#include "Networking/EventQueue.h"
#include "Networking/ClientSidePrediction.h"
#include "Physics/WorldContactListener.h"

struct RenderSystem;
struct UpdateSystem;

class GameCore
{
public:
	GameCore(Core* core, HostType hostType);

	void SpawnPlayer(MobID playerID, const std::string& charName, const Vec2f& tilePos, bool isLocal);

	void PhysicsStep(float deltaTime);

	void Create();
	void Update(float deltaTime);
	void Render();
	void Destroy();

	MobID CreateMobID();
private:
	void SetupServer();

	b2Body* CreateDynamicBoxBody(const Vec2f& position, const Vec2f& size, const Vec2f& footRatio, SensorComponent* groundDetectionComponent);
	void DefineFixtureData(b2FixtureDef* fixtureDef, FixtureUserData* fixtureData);

	void AddRenderSystem(Ref<RenderSystem> system);
	void AddUpdateSystem(Ref<UpdateSystem> system);
public:
	Core* core;
	const std::string mapFilepath;
	entt::registry registry; // ECS registry
	uint64_t frameCounter = 0;
	MobID globalMobID = 0;
	std::unordered_map<MobID, entt::entity> mobs;

	// Physics
	Ref<WorldContactListener> worldContactListener;
	b2World physicsWorld;

	// Networking
	Ref<NetworkHost> host = nullptr;
	Ref<ClientSidePrediction> clientPrediction = nullptr;

	// Rendering
	Ref<TextureManager> textureManager;
	Ref<TextureAtlas> atlas;
	Ref<TileMap> map;
private:
	const HostType m_hostType;
	Array<Ref<RenderSystem>> m_renderSystems;
	Array<Ref<UpdateSystem>> m_updateSystems;

	Array<FixtureUserData*> m_fixturesUserData;
};

