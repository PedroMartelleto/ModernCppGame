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
	textureManager = new TextureManager("Resources/Sprites/");
	atlas = TextureAtlas::FromFile("Resources/Sprites/DungeonTileset/Atlas.meta");
	registry = entt::registry();

	listener = WorldContactListener(this);
	physicsWorld.SetContactListener(&listener);
}

void GameCore::AddRenderSystem(RenderSystem* renderSystem)
{
	m_renderSystems.push_back(renderSystem);
}

void GameCore::AddUpdateSystem(UpdateSystem* updateSystem)
{
	m_updateSystems.push_back(updateSystem);
}

b2Body* GameCore::CreateDynamicBoxBody(const Vec2f& position, const Vec2f& size, const Vec2f& footRatio, GroundDetectionComponent* groundDetectionComponent)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x * PIXELS_TO_METERS, position.y * PIXELS_TO_METERS);
	bodyDef.fixedRotation = true;
	bodyDef.allowSleep = false;

	b2PolygonShape box;
	box.SetAsBox(PIXELS_TO_METERS * size.x / 2, PIXELS_TO_METERS * size.y / 2);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.friction = 0.0f;
	fixtureDef.density = 1.0f;

	auto footSize = size.Multiply(footRatio);
	
	b2PolygonShape footShape;
	footShape.SetAsBox(PIXELS_TO_METERS * footSize.x / 2, PIXELS_TO_METERS * footSize.y / 2, Vec2f(0, PIXELS_TO_METERS * (size.y / 2 + 1.0f)).b2(), 0.0f);

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
	map = new TileMap(physicsWorld, MAP_SCALE, "Resources/Maps/Map1.tmx", textureManager);

	auto mapWidth = map->layers[0]->GetWidth();
	auto mapHeight = map->layers[0]->GetHeight();

	std::ifstream mobDataFile("Resources/GameData/MobData.json");
	auto mobDataJson = json::parse(mobDataFile);
	
	auto mobComponent = MobComponent();
	Serialization::from_json(mobDataJson.at("knight"), mobComponent);

	core->SetWindowSizeAndCenter((int)((float)mapWidth * MAP_SCALE * TILE_SIZE), (int)((float)mapHeight * MAP_SCALE * TILE_SIZE));
	
	auto windowWidth = (float) GetScreenWidth();
	auto windowHeight = (float) GetScreenHeight();

	auto region = atlas->GetAnimFrameRegion("knight_m_idle_anim", 0);
	auto size = Vec2f(region.width * 3, region.height * 3);
	auto heightOffset = 16.0f;
	
	auto player = registry.create();
	auto aabbSize = Vec2f(9 * MAP_SCALE, 16 * MAP_SCALE);

	registry.emplace<GroundDetectionComponent>(player);

	auto* groundDetectionComponent = registry.try_get<GroundDetectionComponent>(player);
	registry.emplace<PhysicsBodyComponent>(player, CreateDynamicBoxBody(Vec2f(380, 200), aabbSize, Vec2f(0.9f, 0.2f), groundDetectionComponent));
	registry.emplace<SpriteComponent>(player, textureManager->Get("DungeonTileset/Atlas.png"), 0, size * -1 + Vec2f(-2.0f, -heightOffset), size, WHITE);
	registry.emplace<TextureRegionComponent>(player, region);
	// registry.emplace<DEBUG_PhysicsBodyDrawComponent>(player, true, true);

	auto& playerInputComponent = registry.emplace<PlayerInputComponent>(player);
	playerInputComponent.JUMP = KEY_W;
	playerInputComponent.MOVE_LEFT = KEY_A;
	playerInputComponent.MOVE_RIGHT = KEY_D;

	registry.emplace<MobComponent>(player, mobComponent);

	// Adds update systems
	AddUpdateSystem(new MobBodyUpdateSystem());
	AddUpdateSystem(new MobInputUpdateSystem());
	AddUpdateSystem(new SpriteBodyUpdateSystem());

	// Adds render systems
	AddRenderSystem(new SpriteRenderSystem());
}

void GameCore::Update(float deltaTime)
{
	for (auto* updateSystem : m_updateSystems)
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
		for (auto layer : map->layers)
		{
			layer->Render(Vec2f::zero, map->mapScale);
		}
	}

	for (auto* renderSystem : m_renderSystems)
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
	for (auto* updateSystem : m_updateSystems)
	{
		delete updateSystem;
	}
	m_updateSystems.clear();

	for (auto* renderSystem : m_renderSystems)
	{
		delete renderSystem;
	}
	m_renderSystems.clear();

	if (textureManager != nullptr)
	{
		textureManager->DestroyAll();
		delete textureManager;
		textureManager = nullptr;
	}

	if (atlas != nullptr)
	{
		delete atlas;
		atlas = nullptr;
	}

	if (map != nullptr)
	{
		map->Destroy();
		delete map;
	}
}
