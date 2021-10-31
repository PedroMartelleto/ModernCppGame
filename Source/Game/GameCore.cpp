#include "GameCore.h"
#include "ECS/Components.h"
#include "ECS/UpdateSystems/MobBodyUpdateSystem.h"
#include "ECS/UpdateSystems/MobInputUpdateSystem.h"
#include "ECS/UpdateSystems/SpriteBodyUpdateSystem.h"
#include "ECS/RenderSystems/SpriteRenderSystem.h"

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

GameCore::GameCore(Core* core) :
	core(core),
	map(nullptr),
	physicsWorld(b2Vec2(0, 15.0f))
{
	textureManager = CreateRef<TextureManager>("Resources/Sprites/");
	atlas = TextureAtlas::FromFile("Resources/Sprites/DungeonTileset/Atlas.meta");
	registry = entt::registry();

	listener = WorldContactListener(this);
	physicsWorld.SetContactListener(&listener);
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

	b2FixtureDef footFixDef;
	footFixDef.shape = &footShape;
	footFixDef.isSensor = true;
	footFixDef.userData.pointer = reinterpret_cast<uintptr_t>(groundDetectionComponent);

	b2Body* body = physicsWorld.CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);
	body->CreateFixture(&footFixDef);

	return body;
}

void GameCore::Create()
{
	map = CreateRef<TileMap>(physicsWorld, Game::MAP_SCALE, "Resources/Maps/Map1.tmx", textureManager);

	std::ifstream mobDataFile("Resources/GameData/MobData.json");
	auto mobDataJson = json::parse(mobDataFile);

	auto mapWidth = map->layers[0]->width;
	auto mapHeight = map->layers[0]->height;
	core->SetWindowSizeAndCenter((int)map->WidthInPixels(), (int)map->HeightInPixels());
	
	auto mobComponent = MobComponent();
	Serialization::from_json(mobDataJson.at("knight"), mobComponent);

	auto windowWidth = (float)Render2D::GetScreenWidth();
	auto windowHeight = (float)Render2D::GetScreenHeight();

	auto region = atlas->GetAnimFrameRegion("knight_m_idle_anim", 0);
	auto size = region.size * Game::MAP_SCALE;
	float heightOffset = 16.0f;
	
	auto player = registry.create();
	auto aabbSize = Vec2f(9 * Game::MAP_SCALE, 16 * Game::MAP_SCALE);

	registry.emplace<GroundDetectionComponent>(player);

	auto* groundDetectionComponent = registry.try_get<GroundDetectionComponent>(player);
	registry.emplace<PhysicsBodyComponent>(player, CreateDynamicBoxBody(Vec2f(380, 200), aabbSize, Vec2f(0.9f, 0.2f), groundDetectionComponent));
	registry.emplace<SpriteComponent>(player, textureManager->Get("DungeonTileset/Atlas.png"), 10000, -size/2.0f + Vec2f(-2.0f, -heightOffset), size, Colors::WHITE);
	registry.emplace<TextureRegionComponent>(player, region);

	auto& playerInputComponent = registry.emplace<PlayerInputComponent>(player);
	playerInputComponent.JUMP = Input::KEY_W;
	playerInputComponent.MOVE_LEFT = Input::KEY_A;
	playerInputComponent.MOVE_RIGHT = Input::KEY_D;

	registry.emplace<MobComponent>(player, mobComponent);

	// Adds update systems
	AddUpdateSystem(CreateRef<MobBodyUpdateSystem>());
	AddUpdateSystem(CreateRef<MobInputUpdateSystem>());
	AddUpdateSystem(CreateRef<SpriteBodyUpdateSystem>());

	// Adds render systems
	AddRenderSystem(CreateRef<SpriteRenderSystem>());
}

void GameCore::Update(float deltaTime)
{
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
	m_updateSystems.clear();
	m_renderSystems.clear();

	if (textureManager != nullptr)
	{
		textureManager->DestroyAll();
		textureManager = nullptr;
	}
}
