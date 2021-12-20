#pragma once

#include "../Engine/Engine.h"
#include "Globals.h"
#include "Networking/NetworkHost.h"
#include "Networking/EventQueue.h"
#include "Networking/ClientSidePrediction.h"
#include "Physics/WorldContactListener.h"
#include "GameData.h"
#include "../Engine/Threads/JobSystem.h"

namespace ZPlanes
{
	const float DEBUG = 8.5f;
	const float UI = 7.5f;
	const float MOBS = 5.5f;
	const float MAP = 4.5f;
	const float PROJECTILES = 3.5f;
	const float BACKGROUND = 2.5f;
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
	/// Called once a winner has been decided.
	/// </summary>
	/// <param name="winner">Player number of the winner.</param>
	void SetWinner(int winner);

	/// <summary>
	/// Called when the game starts.
	/// </summary>
	void Create();
	
	/// <summary>
	/// Called every frame.
	/// </summary>
	void Update(float deltaTime);
	
	/// <summary>
	/// Called every frame after Update.
	/// </summary>
	void Render();

	/// <summary>
	/// Called when the game ends.
	/// </summary>
	void Destroy();

	/// <summary>
	/// Removes the body of the given entity when it is safe.
	/// </summary>
	void QueueBodyForRemoval(entt::entity entity);

	MobID CreateMobID();
	int8_t CreatePlayerIndex();

	void DefineFixtureData(b2FixtureDef* fixtureDef, FixtureUserData* fixtureData);

	inline Ref<TextureAtlas> GetAtlas(ResourceID alias)
	{
		return resourceManager->GetAtlas(GameData::resourceIdToPath.at(alias), false);
	}

private:
	/// <summary>
	/// Setup required only for servers.
	/// </summary>
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

	/// <summary>
	/// Maps a mob id to an entity handle.
	/// </summary>
	std::unordered_map<MobID, entt::entity> mobs;
	
	/// <summary>
	/// List of the entity handles for the players.
	/// </summary>
	std::vector<entt::entity> players;

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
	/// <summary>
	/// The host type (server or client).
	/// </summary>
	const HostType m_hostType;

	/// <summary>
	/// Arrays for delayed removal of certain objects.
	/// </summary>
	Array<entt::entity> m_bodiesForRemoval;
	Array<FixtureUserData*> m_fixturesUserData;
};

