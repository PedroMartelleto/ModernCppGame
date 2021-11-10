#pragma once

#include "../Engine/Engine.h"
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

	void PhysicsStep(float deltaTime);

	void Create();
	void Update(float deltaTime);
	void Render();
	void Destroy();

	MobID CreateMobID();

	void DefineFixtureData(b2FixtureDef* fixtureDef, FixtureUserData* fixtureData);
private:
	void SetupServer();
public:
	Core* core;
	const std::string mapFilepath;
	entt::registry registry; // ECS registry
	uint64_t frameCounter = 0;
	MobID globalMobID = 0;
	std::unordered_map<MobID, entt::entity> mobs;
	int localPlayerCount = 0;

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
	Array<FixtureUserData*> m_fixturesUserData;
};

