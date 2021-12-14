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

namespace ZPlanes
{
	const float UI = 0.5f;
	const float MOBS = 2.5f;
	const float MAP = 7.5f;
	const float BACKGROUND = 8.5f;
};

enum class GameState
{
	MAIN_MENU = 0,
	WIN_SCREEN = 1,
	IN_GAME = 2
};

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

	void SetWinner(int winner);

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
	static Ref<Font> defaultFont;

	GameState gameState;
	int winner = 0;

	int8_t playerIndex = -1;

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
	Ref<ResourceManager> resourceManager;
	Ref<TileMap> map;

	/// <summary>
	/// Gets the number of mobs that are not a player (eg. enemies).
	/// </summary>
	/// <returns>Integer with the count.</returns>
	int GetNonPlayerCount() const;
private:
	const HostType m_hostType;
	Array<b2WeldJointDef> m_weldJoints;
	Array<FixtureUserData*> m_fixturesUserData;
};

