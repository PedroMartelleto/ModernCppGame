#include "GameCore.h"
#include "ECS/Components.h"
#include "ECS/RenderSystems.h"
#include "ECS/UpdateSystems.h"
#include "GameData.h"
#include "Networking/EventHandler.h"

Ref<Font> GameCore::defaultFont = nullptr;

GameCore::GameCore(Core* core, HostType hostType) :
	core(core),
	map(nullptr),
	physicsWorld(b2Vec2(0, 15.0f)),
	m_hostType(hostType),
	mapFilepath("Resources/Maps/Map2.tmx"),
	gameState(GameState::MAIN_MENU)
{
	assert(m_hostType == HostType::CLIENT || m_hostType == HostType::SERVER);

	GameData::Create("Resources/GameData/");

	resourceManager = CreateRef<ResourceManager>("Resources/Sprites/");
	registry = entt::registry();

	worldContactListener = CreateRef<WorldContactListener>(this);
	physicsWorld.SetContactListener(worldContactListener.get());
}

void GameCore::SetWinner(int winner)
{
	gameState = GameState::WIN_SCREEN;
	this->winner = winner;
	
	std::vector<MobID> mobsToRemove;
	auto nextSpawns = map->GetSpawns(4);

	// Resets the health of the players and removes all other mobs from the map.
	for (auto entity : registry.view<MobComponent>())
	{
		auto& mob = registry.get<MobComponent>(entity);

		if (mob.IsPlayer())
		{
			mob.Reset(registry.try_get<ProjectileInventoryComponent>(entity));
			
			auto* body = registry.try_get<PhysicsBodyComponent>(entity);
			if (body != nullptr)
			{
				body->body->SetTransform(Vec2fToB2(nextSpawns.back() * Game::PIXELS_TO_METERS), 0.0f);
			}
			nextSpawns.pop_back();
		}
		else
		{
			mobsToRemove.push_back(mob.mobID);
		}
	}

	// Enqueues the remove mobs event.
	host->eventQueue.Enqueue(EventType::RemoveMobs, CreateRef<RemoveMobsEvent>(mobsToRemove));
}

int GameCore::GetNonPlayerCount() const
{
	assert(playerIndex >= 0);
	return mobs.size() - playerIndex - 1;
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

int8_t GameCore::CreatePlayerIndex()
{
	playerIndex += 1;
	return playerIndex;
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
	defaultFont = CreateRef<Font>("DefaultFont", resourceManager->GetTexture("DefaultFont/SpriteSheet.png", true),
								  20, 20, " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_ abcdefghijklmnopqrstuvwxyz{|}~                         ");
	assert(defaultFont->characters.size() == 15 * 8); // Ensures we actually have the right number of characters in the font specs

	// Setups networking
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
			auto* fixture = jointDef.bodyB->GetFixtureList();
			jointDef.bodyB->SetAwake(false);

			while (fixture != nullptr)
			{
				fixture->SetSensor(true);
				fixture = fixture->GetNext();
			}
		}

		m_weldJoints.clear();
	}
}

void GameCore::Update(float deltaTime)
{
	if (gameState == GameState::IN_GAME)
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
	// Simple transition states
	else if (gameState == GameState::MAIN_MENU)
	{
		if (Input::IsKeyDown(Input::KEY_RETURN))
		{
			gameState = GameState::IN_GAME;
		}
	}
	else if (gameState == GameState::WIN_SCREEN)
	{
		if (Input::IsKeyDown(Input::KEY_RETURN))
		{
			gameState = GameState::IN_GAME;
		}
	}
}

void GameCore::Render()
{
	Render2D::SetClearColor(Color4f(95 / 255.0f, 80 / 255.0f, 115 /255.0f, 1.0f) * 1.4f);

	if (gameState == GameState::IN_GAME && map != nullptr)
	{
		// Draws the tile map
		float mapZIndex = ZPlanes::MAP;
		for (auto layer : map->layers)
		{
			layer->Render(Vec2f(0, 0), map->mapScale, mapZIndex);
			mapZIndex += 0.05f;
		}

		for (const auto& renderSystem : RenderSystems::renderSystems)
		{
			renderSystem(this);
		}

		auto bg = resourceManager->GetTexture("Desert/background/BG-mountains.png", true);
		Render2D::DrawRect(Vec2f(0, -128), 0, bg->GetSize() * 2.1f * map->mapScale, ZPlanes::BACKGROUND, bg, Color4f(1.05f, 0.95f, 1.05f, 1.0f));

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
	else if (gameState == GameState::MAIN_MENU)
	{
		Render2D::DrawLineOfText(Render2D::GetScreenCenter() - Vec2f(0.0f, 128), 2.0f, "PLATFORMER GAME", ZPlanes::UI, defaultFont);
		Render2D::DrawLineOfText(Render2D::GetScreenCenter(), 2.0f, "PRESS 'ENTER' TO PLAY", ZPlanes::UI, defaultFont);
	}
	else if (gameState == GameState::WIN_SCREEN)
	{
		Render2D::DrawLineOfText(Render2D::GetScreenCenter() - Vec2f(0.0f, 128), 2.0f, "PLAYER " + std::to_string(winner) + " WINS!", ZPlanes::UI, defaultFont);
		Render2D::DrawLineOfText(Render2D::GetScreenCenter(), 2.0f, "PRESS 'ENTER' TO PLAY AGAIN", ZPlanes::UI, defaultFont);
	}
}

void GameCore::Destroy()
{
	UpdateSystems::Cleanup();

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
