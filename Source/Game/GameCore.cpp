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
	physicsWorld(b2Vec2(0, 19.0f)),
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

	// Resets the health of the players and removes all other mobs from the map.
	for (auto entity : registry.view<MobComponent>())
	{
		auto& mob = registry.get<MobComponent>(entity);

		if (!mob.IsPlayer())
		{
			mobsToRemove.push_back(mob.mobID);
		}
	}

	// Enqueues the remove mobs event.
	host->eventQueue.Enqueue(EventType::RemoveMobs, CreateRef<RemoveMobsEvent>(mobsToRemove));
	host->eventQueue.Enqueue(EventType::ResetMap, nullptr);
}

int GameCore::GetNonPlayerCount() const
{
	assert(playerIndex >= 0);
	return mobs.size() - playerIndex - 1;
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

void GameCore::QueueBodyForRemoval(entt::entity entity)
{
	m_bodiesForRemoval.push_back(entity);
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
	std::vector<char> chars = { ' ', '!', '\"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5',
								'6', '7', '8', '9', ':', ';' , '<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
								'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']',
								'^', '_', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
								's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{' , '|', '}', '~' };
	defaultFont = CreateRef<Font>("DefaultFont", resourceManager->GetTexture("DefaultFont/SpriteSheet.png", true),
								  20, 20, chars);

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

	if (m_bodiesForRemoval.size() > 0)
	{
		// Removes bodies from the world
		for (auto entityHandle : m_bodiesForRemoval)
		{
			auto* body = registry.try_get<PhysicsBodyComponent>(entityHandle);
			if (body != nullptr)
			{
				physicsWorld.DestroyBody(body->body);
				registry.remove<PhysicsBodyComponent>(entityHandle);
			}
		}
		m_bodiesForRemoval.clear();
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
