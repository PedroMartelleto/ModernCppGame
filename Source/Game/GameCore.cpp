#include "GameCore.h"
#include "ECS/Components.h"
#include "ECS/RenderSystems.h"
#include "ECS/UpdateSystems.h"
#include "GameData.h"
#include "Networking/EventHandler.h"

GameCore::GameCore(Core* core, HostType hostType) :
	core(core),
	map(nullptr),
	physicsWorld(b2Vec2(0, 15.0f)),
	m_hostType(hostType),
	mapFilepath("Resources/Maps/Map1.tmx")
{
	assert(m_hostType == HostType::CLIENT || m_hostType == HostType::SERVER);

	GameData::Create("Resources/GameData/");

	resourceManager = CreateRef<ResourceManager>("Resources/Sprites/");
	registry = entt::registry();

	worldContactListener = CreateRef<WorldContactListener>(this);
	physicsWorld.SetContactListener(worldContactListener.get());
}

void GameCore::AddJointWhenPossible(const b2WeldJointDef& jointDef)
{
	m_weldJoints.push_back(jointDef);
}

MobID GameCore::CreateMobID()
{
	assert(m_hostType == HostType::SERVER);
	globalMobID += 1;
	return globalMobID;
}

void GameCore::DefineFixtureData(b2FixtureDef* fixtureDef, FixtureUserData* fixtureData)
{
	m_fixturesUserData.push_back(fixtureData);
	fixtureDef->userData.pointer = reinterpret_cast<uintptr_t>(m_fixturesUserData.back());
}

void GameCore::SetupServer()
{
	host->eventQueue.Enqueue(EventType::Map, CreateRef<MapDataEvent>(MapDataEvent{ Utils::LoadFile(mapFilepath) }));

	host->eventQueue.Enqueue(EventType::SpawnPlayers, CreateRef<SpawnPlayersEvent>(
		SpawnPlayersEvent {
			{ CreateMobID(), CreateMobID() }, { HostType::SERVER, HostType::SERVER },
			{ "knight_m", "elf_m" }
		}
	));
}

void GameCore::Create()
{
	host = CreateRef<NetworkHost>(this, m_hostType, "127.0.0.1");

	if (m_hostType == HostType::SERVER)
	{
		SetupServer();
	}
	else if (m_hostType == HostType::CLIENT)
	{
		clientPrediction = CreateRef<ClientSidePrediction>(this);
	}
}

void GameCore::PhysicsStep(float deltaTime)
{
	physicsWorld.Step(deltaTime, 6, 2);

	// Adds joints to the world (if applicable)
	if (m_weldJoints.size() > 0)
	{
		for (const auto& jointDef : m_weldJoints)
		{
			physicsWorld.CreateJoint(&jointDef);
		}

		m_weldJoints.clear();
	}
}

void GameCore::Update(float deltaTime)
{
	// Poll events from the network
	host->PollEvents();

	EventHandler::HandleEvents(this);

	// If there is no map, there is nothing to update
	if (map == nullptr) return;

	// Update client prediction
	if (clientPrediction != nullptr) clientPrediction->Update(deltaTime);

	// Calls update systems and runs step in physics world
	for (const auto& updateSystem : UpdateSystems::updateSystems)
	{
		updateSystem(this, deltaTime);
	}

	PhysicsStep(deltaTime);

	// Server streams a world snapshot to all clients
	if (frameCounter % GameData::GetWorldSnapshotTickRate() == 0 && m_hostType == HostType::SERVER)
	{
		WorldSnapshotEvent event;
		event.Save(this);
		host->SendPacket(PacketData{ { Utils::ToJSON(event) }, { EventType::WorldSnapshot } }, 0, 0);
	}

	frameCounter += 1;
}

void GameCore::Render()
{
	// Draws the tile map
	if (map != nullptr)
	{
		int mapZIndex = 100;
		for (auto layer : map->layers)
		{
			layer->Render(Vec2f(0, 0), map->mapScale, mapZIndex);
			mapZIndex += 10;
		}

		for (const auto& renderSystem : RenderSystems::renderSystems)
		{
			renderSystem(this);
		}
	}

#ifdef _DEBUG
	// Renders debug collision boxes
	for (auto entity : registry.view<PhysicsBodyComponent, DEBUG_PhysicsBodyDrawComponent>())
	{
		auto& body = registry.get<PhysicsBodyComponent>(entity);
		auto& draw = registry.get<DEBUG_PhysicsBodyDrawComponent>(entity);

		if (draw.drawAABB)
		{
			core->DEBUG_DrawBodyAABB(body.body, draw.aabbColor);
		}

		if (draw.drawPoly)
		{
			core->DEBUG_DrawBody(body.body, draw.polyColor);
		}
	}
#endif
}

void GameCore::Destroy()
{
	host->Disconnect();

	if (resourceManager != nullptr)
	{
		resourceManager->DestroyAll();
		resourceManager = nullptr;
	}

	for (auto* fixtureData : m_fixturesUserData)
	{
		delete fixtureData;
	}
	m_fixturesUserData.clear();
}
