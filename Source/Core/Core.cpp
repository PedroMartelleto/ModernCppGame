#include "Core.h"
#include "../Render/TextureAtlas.h"
#include "../Map/TileMap.h"
#include "../Entity/GameEntity.h"
#include "GameLoop.h"
#include <chrono>
#include <thread>

Core::Core(int targetFPS) : map(nullptr), physicsWorld(b2Vec2(0, GameEntity::gravity))
{
	m_uidCounter = 0;
	m_frameTime = 1.0f / (float)targetFPS;
	m_isRunning = false;
	m_textureManager = new TextureManager("Resources/Sprites/");
	m_atlas = TextureAtlas::FromFile("Resources/Sprites/DungeonTileset/Atlas.meta");
	m_gameLoop = new GameLoop(this);
}

UID Core::CreateUID()
{
	m_uidCounter += 1;
	return m_uidCounter;
}

TextureAtlas* Core::GetTextureAtlas()
{
	return m_atlas;
}

TextureManager* Core::GetTextureManager()
{
	return m_textureManager;
}

void Core::Create()
{
	InitWindow(0, 0, "CppGame");

	m_gameLoop->Create();

	for (auto iterator = m_entities.begin(); iterator != m_entities.end(); ++iterator)
	{
		(*iterator).second->Create();
	}
}

void Core::Destroy()
{
	for (auto iterator = m_entities.begin(); iterator != m_entities.end(); ++iterator)
	{
		(*iterator).second->Destroy();
		delete (*iterator).second;
	}
	m_entities.clear();

	m_gameLoop->Destroy();
	m_textureManager->DestroyAll();

	delete m_textureManager;
	delete m_atlas;
	delete m_gameLoop;

	if (map != nullptr)
	{
		map->Destroy();
		delete map;
	}

	CloseWindow();
}

void Core::SetWindowSizeAndCenter(int windowWidth, int windowHeight) const
{
	SetWindowSize(windowWidth, windowHeight);
	SetWindowPosition((GetMonitorWidth(0) - windowWidth) / 2, (GetMonitorHeight(0) - windowHeight) / 2);
}

void Core::Run()
{
	double lastTime = GetTime();
	double frameCounter = 0;
	double unprocessedTime = 0;
	int frames = 0;

	m_isRunning = true;
	
	while (m_isRunning)
	{
		bool render = false;

		// Computes the time at the start of the frame and the passed time since the last frame
		double startTime = GetTime();
		double passedTime = startTime - lastTime;
		lastTime = startTime;

		unprocessedTime += passedTime;
		frameCounter += passedTime;

		// Displays FPS
		if (frameCounter >= 3.0)
		{
			double totalTime = ((1000.0 * frameCounter) / ((double)frames));
			LOGGER_MESSAGE("Frame Time: " + std::to_string(round(totalTime*100)/100.0) + " ms");
			frames = 0;
			frameCounter = 0;
		}
		
		// While there is time to process...
		while (unprocessedTime > m_frameTime)
		{
			if (WindowShouldClose())
			{
				m_isRunning = false;
			}

			Update((float)m_frameTime);
			render = true; // Since we updated, re-render needs to happen as well

			unprocessedTime -= m_frameTime;
		}

		if (render)
		{
			Render();
			frames++;
		}
		else
		{
			// Nothing to render, sleep for 1 ms
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	m_isRunning = false;
}

void Core::AddEntity(GameEntity* entity)
{
	m_entities[entity->uid] = entity;
}

GameEntity* Core::GetEntity(UID uid)
{
	return m_entities[uid];
}

void Core::DestroyEntity(UID uid)
{
	m_entities[uid]->Destroy();
	delete m_entities[uid];
	m_entities.erase(uid);
}

void Core::Update(float deltaTime)
{
	physicsWorld.Step(deltaTime, 6, 2);

	m_gameLoop->Update(deltaTime);

	for (auto iterator = m_entities.begin(); iterator != m_entities.end(); ++iterator)
	{
		auto entity = (*iterator).second;
		entity->Update(deltaTime);
	}
}

void Core::DEBUG_DrawRect(const Vec2f& pos, const Vec2f& size, const Color& color)
{
	m_debugRects.push_back({ CreateRectangle(pos, size), color });
}

void Core::DEBUG_DrawBodyAABB(b2Body* body, const Color& color)
{
	b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		for (int childIndex = 0; childIndex < fixture->GetShape()->GetChildCount(); ++childIndex)
		{
			auto aabb = fixture->GetAABB(childIndex);
			auto bottomRight = Vec2f(aabb.upperBound) * METERS_TO_PIXELS;
			auto topLeft = Vec2f(aabb.lowerBound) * METERS_TO_PIXELS;

			m_debugRects.push_back({ CreateRectangle(topLeft, (bottomRight - topLeft).Abs()), color });
		}
		fixture = fixture->GetNext();
	}
}

void Core::DEBUG_DrawBody(b2Body* body, const Color& color)
{
	b2Fixture* fixture = body->GetFixtureList();
	DebugLineInfo line;
	line.color = color;
	line.thickness = 3.0f;

	while (fixture != NULL)
	{
		b2PolygonShape* polygonShape = (b2PolygonShape*)fixture->GetShape();
		for (int i = 1; i < polygonShape->m_count; ++i)
		{
			line.start = Vec2f(polygonShape->m_vertices[i-1]) * METERS_TO_PIXELS;
			line.end = Vec2f(polygonShape->m_vertices[i]) * METERS_TO_PIXELS;
			m_debugLines.push_back(line);
		}
		line.start = Vec2f(polygonShape->m_vertices[polygonShape->m_count-1]) * METERS_TO_PIXELS;
		line.end = Vec2f(polygonShape->m_vertices[0]) * METERS_TO_PIXELS;
		m_debugLines.push_back(line);
		fixture = fixture->GetNext();
	}
}

void Core::Render()
{
	BeginDrawing();
	ClearBackground(DARKGRAY);

	if (map != nullptr)
	{
		m_gameLoop->Render();

		for (auto layer : map->layers)
		{
			layer->Render(Vec2f::zero, map->mapScale);
		}

		for (auto iterator = m_entities.begin(); iterator != m_entities.end(); ++iterator)
		{
			auto entity = (*iterator).second;
			entity->Render();
		}
	}

	for (auto rect : m_debugRects)
	{
		auto rectToDraw = rect.first;
		DrawRectangle((int) rectToDraw.x, (int) rectToDraw.y, (int) rectToDraw.width, (int) rectToDraw.height, rect.second);
	}

	for (auto line : m_debugLines)
	{
		DrawLineEx(line.start.raylib(), line.end.raylib(), line.thickness, line.color);
	}

	m_debugRects.clear();
	
	EndDrawing();
}