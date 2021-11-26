#pragma once

#include "../Engine/Engine.h"
#include "Globals.h"
#include "Networking/NetworkHost.h"
#include "Networking/EventQueue.h"
#include "Networking/ClientSidePrediction.h"
#include "Physics/WorldContactListener.h"
#include "GameData.h"
#include "../Engine/Threads/JobSystem.h"

struct RenderSystem;
struct UpdateSystem;

class GameCore
{
public:
	GameCore(Core* core, HostType hostType);

	void PhysicsStep(float deltaTime);

	/// <summary>
	/// Adds a joint to the physics world as soon as possible (may be called during callbacks to avoid adding a joint when the world is locked).
	/// </summary>
	/// <param name="jointDef"></param>
	void AddJointWhenPossible(const b2WeldJointDef& jointDef);

	void Create();
	void Update(float deltaTime);
	void Render();
	void Destroy();

	MobID CreateMobID();
	int8_t CreatePlayerIndex();

	void DefineFixtureData(b2FixtureDef* fixtureDef, FixtureUserData* fixtureData);

	inline Ref<TextureAtlas> GetAtlas(ResourceID alias)
	{
		return resourceManager->GetAtlas(GameData::resourceIdToPath.at(alias), false);
	}

private:
	void SetupServer();
public:
	int8_t playerIndex = -1;

	Core* core;
	const std::string mapFilepath;
	entt::registry registry; // ECS registry
	uint64_t frameCounter = 0;
	MobID globalMobID = 0;
	std::unordered_map<MobID, entt::entity> mobs;
	int localPlayerCount = 0;

	Ref<JobSystem> jobSystem;

	// Physics
	Ref<WorldContactListener> worldContactListener;
	b2World physicsWorld;

	// Networking
	Ref<NetworkHost> host = nullptr;
	Ref<ClientSidePrediction> clientPrediction = nullptr;

	// Rendering
	Ref<ResourceManager> resourceManager;
	Ref<TileMap> map;
private:
	const HostType m_hostType;
	Array<b2WeldJointDef> m_weldJoints;
	Array<FixtureUserData*> m_fixturesUserData;
};

