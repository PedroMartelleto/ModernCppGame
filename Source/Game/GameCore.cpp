#include "GameCore.h"
#include "ECS/Components.h"
#include "ECS/UpdateSystems/MobBodyUpdateSystem.h"
#include "ECS/UpdateSystems/MobInputUpdateSystem.h"
#include "ECS/UpdateSystems/SpriteBodyUpdateSystem.h"
#include "ECS/RenderSystems/SpriteRenderSystem.h"
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

	textureManager = CreateRef<TextureManager>("Resources/Sprites/");
	atlas = TextureAtlas::FromFile("Resources/Sprites/DungeonTileset/Atlas.meta");
	registry = entt::registry();

	worldContactListener = CreateRef<WorldContactListener>(this);
	physicsWorld.SetContactListener(worldContactListener.get());
}

void GameCore::AddRenderSystem(Ref<RenderSystem> renderSystem)
{
	m_renderSystems.push_back(renderSystem);
}

void GameCore::AddUpdateSystem(Ref<UpdateSystem> updateSystem)
{
	m_updateSystems.push_back(updateSystem);
}

MobID GameCore::CreateMobID()
{
	assert(m_hostType == HostType::SERVER);
	globalMobID += 1;
	return globalMobID;
}

b2Body* GameCore::CreateDynamicBoxBody(const Vec2f& position, const Vec2f& size, const Vec2f& footRatio, SensorComponent* sensorComponent)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x * Game::PIXELS_TO_METERS, position.y * Game::PIXELS_TO_METERS);
	bodyDef.fixedRotation = true;
	bodyDef.allowSleep = false;

	b2PolygonShape box;
	box.SetAsBox(Game::PIXELS_TO_METERS * size.x / 2, Game::PIXELS_TO_METERS * size.y / 2);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.friction = 0.0f;
	fixtureDef.density = 1.0f;

	auto footSize = size * footRatio;
	
	b2PolygonShape footShape;
	footShape.SetAsBox(Game::PIXELS_TO_METERS * footSize.x / 2, Game::PIXELS_TO_METERS * footSize.y / 2, b2Vec2(0, Game::PIXELS_TO_METERS * (size.y / 2 + 1.0f)), 0.0f);

	b2Body* body = physicsWorld.CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);

	if (sensorComponent != nullptr)
	{
		b2FixtureDef footFixDef;
		footFixDef.shape = &footShape;
		footFixDef.isSensor = true;
		DefineFixtureData(&footFixDef, new FixtureUserData{ sensorComponent });
		body->CreateFixture(&footFixDef);
	}

	return body;
}

void GameCore::DefineFixtureData(b2FixtureDef* fixtureDef, FixtureUserData* fixtureData)
{
	m_fixturesUserData.push_back(fixtureData);
	fixtureDef->userData.pointer = reinterpret_cast<uintptr_t>(m_fixturesUserData.back());
}

void GameCore::SpawnPlayer(MobID playerID, const std::string& charName, const Vec2f& tilePos, bool isLocal)
{
	if (map == nullptr)
	{
		DEBUG_LOG("CORE", LOG_ERROR, "Map cannot be nullptr when SpawnPlayer is called.");
		return;
	}

	auto mobComponent = GameData::GetMobData(charName, playerID);

	auto windowWidth = (float)Render2D::GetScreenWidth();
	auto windowHeight = (float)Render2D::GetScreenHeight();

	auto region = atlas->GetAnimFrameRegion(charName + "_m_idle_anim", 0);

	auto player = registry.create();
	auto aabbSize = Vec2f(9, 16);

	registry.emplace<SensorComponent>(player);

	auto* sensorComponent = registry.try_get<SensorComponent>(player);
	auto dynamicBody = CreateDynamicBoxBody(tilePos * map->scaledTileSize, aabbSize * Game::MAP_SCALE,
											Vec2f(0.9f, 0.2f), sensorComponent);

	registry.emplace<PhysicsBodyComponent>(player, dynamicBody);
	registry.emplace<SpriteComponent>(player, textureManager->Get("DungeonTileset/Atlas.png"), 100,
		(Vec2f(-0.3f, -5) - region.size / 2.0f) * Game::MAP_SCALE, region.size * Game::MAP_SCALE, Colors::WHITE);
	registry.emplace<TextureRegionComponent>(player, region);

	if (isLocal)
	{
		auto input = GameData::CreateDefaultBinding(0);
		registry.emplace<LocalInputComponent>(player, input);
	}

	registry.emplace<MobComponent>(player, mobComponent);
	mobs[mobComponent.mobID] = player;
}

void GameCore::SetupServer()
{
	host->eventQueue.Enqueue(EventType::Map, CreateRef<MapDataEvent>(MapDataEvent{ Utils::LoadFile(mapFilepath) }));

	host->eventQueue.Enqueue(EventType::SpawnPlayer, CreateRef<SpawnPlayerEvent>(SpawnPlayerEvent{ CreateMobID(), HostType::SERVER, 9, 8, "knight" }));
	host->eventQueue.Enqueue(EventType::SpawnPlayer, CreateRef<SpawnPlayerEvent>(SpawnPlayerEvent{ CreateMobID(), HostType::SERVER, 14, 8, "elf" }));
}

void GameCore::Create()
{
	// Adds update systems
	AddUpdateSystem(CreateRef<MobBodyUpdateSystem>());
	AddUpdateSystem(CreateRef<MobInputUpdateSystem>());
	AddUpdateSystem(CreateRef<SpriteBodyUpdateSystem>());

	// Adds render systems
	AddRenderSystem(CreateRef<SpriteRenderSystem>());

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
	for (auto updateSystem : m_updateSystems)
	{
		updateSystem->Update(this, deltaTime);
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
		int mapZIndex = 10;
		for (auto layer : map->layers)
		{
			layer->Render(Vec2f(0, 0), map->mapScale, mapZIndex);
			mapZIndex += 1;
		}

		for (auto renderSystem : m_renderSystems)
		{
			renderSystem->Render(this);
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

	m_updateSystems.clear();
	m_renderSystems.clear();

	if (textureManager != nullptr)
	{
		textureManager->DestroyAll();
		textureManager = nullptr;
	}

	for (auto* fixtureData : m_fixturesUserData)
	{
		delete fixtureData;
	}
	m_fixturesUserData.clear();
}
