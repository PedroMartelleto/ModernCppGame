#include "GameCore.h"
#include "ECS/Components.h"
#include "ECS/UpdateSystems/MobBodyUpdateSystem.h"
#include "ECS/UpdateSystems/MobInputUpdateSystem.h"
#include "ECS/UpdateSystems/SpriteBodyUpdateSystem.h"
#include "ECS/RenderSystems/SpriteRenderSystem.h"
#include "GameData.h"

int contactCount = 0;

struct WorldContactListener : public b2ContactListener
{
	GameCore* gameCore;

	WorldContactListener(GameCore* gameCore) : gameCore(gameCore) {}

	void BeginContact(b2Contact* contact) override
	{
		auto fixtureA = contact->GetFixtureA();
		auto fixtureB = contact->GetFixtureB();

		if (fixtureA->GetUserData().pointer == NULL && fixtureB->GetUserData().pointer == NULL) return;

		if (fixtureA->IsSensor() ^ fixtureB->IsSensor())
		{
			contactCount += 1;
		}

		auto userData = fixtureA->GetUserData().pointer == NULL ? fixtureB->GetUserData().pointer : fixtureA->GetUserData().pointer;
		reinterpret_cast<GroundDetectionComponent*>(userData)->isGrounded = contactCount > 0;
	}

	void EndContact(b2Contact* contact) override
	{
		auto fixtureA = contact->GetFixtureA();
		auto fixtureB = contact->GetFixtureB();

		if (fixtureA->GetUserData().pointer == NULL && fixtureB->GetUserData().pointer == NULL) return;

		if (fixtureA->IsSensor() ^ fixtureB->IsSensor())
		{
			contactCount -= 1;
		}

		auto userData = fixtureA->GetUserData().pointer == NULL ? fixtureB->GetUserData().pointer : fixtureA->GetUserData().pointer;
		reinterpret_cast<GroundDetectionComponent*>(userData)->isGrounded = contactCount > 0;
	}
};

WorldContactListener listener = WorldContactListener(nullptr);

GameCore::GameCore(Core* core, HostType hostType) :
	core(core),
	map(nullptr),
	physicsWorld(b2Vec2(0, 15.0f)),
	m_hostType(hostType)
{
	GameData::Create("Resources/GameData/");
	DEBUG_LOG("THREAD", LOG_MSG, "Number of parallel threads: %d", std::thread::hardware_concurrency());

	textureManager = CreateRef<TextureManager>("Resources/Sprites/");
	atlas = TextureAtlas::FromFile("Resources/Sprites/DungeonTileset/Atlas.meta");
	registry = entt::registry();

	listener = WorldContactListener(this);
	physicsWorld.SetContactListener(&listener);

	host = CreateRef<NetworkHost>(hostType, "127.0.0.1");
}

void GameCore::AddRenderSystem(Ref<RenderSystem> renderSystem)
{
	m_renderSystems.push_back(renderSystem);
}

void GameCore::AddUpdateSystem(Ref<UpdateSystem> updateSystem)
{
	m_updateSystems.push_back(updateSystem);
}

b2Body* GameCore::CreateDynamicBoxBody(const Vec2f& position, const Vec2f& size, const Vec2f& footRatio, GroundDetectionComponent* groundDetectionComponent)
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

	if (groundDetectionComponent != nullptr)
	{
		b2FixtureDef footFixDef;
		footFixDef.shape = &footShape;
		footFixDef.isSensor = true;
		footFixDef.userData.pointer = reinterpret_cast<uintptr_t>(groundDetectionComponent);
		body->CreateFixture(&footFixDef);
	}

	return body;
}

void GameCore::SpawnPlayer(const std::string& charName, const Vec2f& tilePos, bool isLocal)
{
	auto mobComponent = GameData::GetMobData(charName);

	auto windowWidth = (float)Render2D::GetScreenWidth();
	auto windowHeight = (float)Render2D::GetScreenHeight();

	auto region = atlas->GetAnimFrameRegion(charName + "_m_idle_anim", 0);

	auto player = registry.create();
	auto aabbSize = Vec2f(9, 16);

	registry.emplace<GroundDetectionComponent>(player);

	auto* groundDetectionComponent = registry.try_get<GroundDetectionComponent>(player);
	auto dynamicBody = CreateDynamicBoxBody(tilePos * map->scaledTileSize, aabbSize * Game::MAP_SCALE,
									Vec2f(0.9f, 0.2f), isLocal ? groundDetectionComponent : nullptr);

	registry.emplace<PhysicsBodyComponent>(player, dynamicBody);
	registry.emplace<SpriteComponent>(player, textureManager->Get("DungeonTileset/Atlas.png"), 100,
		(Vec2f(-0.3f, -5) - region.size / 2.0f) * Game::MAP_SCALE, region.size * Game::MAP_SCALE, Colors::WHITE);
	registry.emplace<TextureRegionComponent>(player, region);

	if (isLocal)
	{
		auto input = GameData::GetDefaultPlayerInput(0);
		registry.emplace<PlayerInputComponent>(player, input);
	}

	registry.emplace<MobComponent>(player, mobComponent);
}

void GameCore::Create()
{
	map = CreateRef<TileMap>(physicsWorld, Game::MAP_SCALE, "Resources/Maps/Map1.tmx", textureManager);
	core->SetWindowSizeAndCenter((int)map->WidthInPixels(), (int)map->HeightInPixels());
	
	SpawnPlayer("lizard", Vec2f(9, 8), m_hostType == HostType::CLIENT);
	SpawnPlayer("wizzard", Vec2f(14, 8), m_hostType != HostType::CLIENT);

	// Adds update systems
	AddUpdateSystem(CreateRef<MobBodyUpdateSystem>());
	AddUpdateSystem(CreateRef<MobInputUpdateSystem>());
	AddUpdateSystem(CreateRef<SpriteBodyUpdateSystem>());

	// Adds render systems
	AddRenderSystem(CreateRef<SpriteRenderSystem>());
}

void GameCore::Update(float deltaTime)
{
	host->Update(deltaTime);

	for (auto updateSystem : m_updateSystems)
	{
		updateSystem->Update(this, deltaTime);
	}

	physicsWorld.Step(deltaTime, 6, 2);
}

void GameCore::Render()
{
	// Draws the tile map
	if (map != nullptr)
	{
		int zIndex = 10;
		for (auto layer : map->layers)
		{
			layer->Render(Vec2f(0, 0), map->mapScale, zIndex);
			zIndex += 1;
		}
	}

	for (auto renderSystem : m_renderSystems)
	{
		renderSystem->Render(this);
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
}
