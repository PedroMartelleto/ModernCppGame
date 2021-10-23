#include "Core.h"
#include "../Render/TextureAtlas.h"
#include "GameLoop.h"
#include <chrono>
#include <thread>

Core::Core(int targetFPS)
{
	m_uidCounter = 0;
	m_windowWidth = -1;
	m_windowHeight = -1;
	m_frameTime = 1.0f / (float)targetFPS;
	m_isRunning = false;
	m_textureManager = new TextureManager("Resources/");
	m_atlas = TextureAtlas::FromFile("Resources/DungeonTileset/Atlas.meta");
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
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(0, 0, "CppGame");

	int monitorWidth = GetMonitorWidth(0);
	int monitorHeight = GetMonitorHeight(0);

	m_windowWidth = (int)(monitorWidth * 0.8);
	m_windowHeight = (int)(monitorHeight * 0.8);

	SetWindowSize(m_windowWidth, m_windowHeight);
	SetWindowPosition((monitorWidth - m_windowWidth) / 2, (monitorHeight - m_windowHeight) / 2);

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

	CloseWindow();
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
			LOGGER_MESSAGE("Frame Time: " + std::to_string(roundf(totalTime*100)/100.0f) + " ms");
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
	m_entities[entity->GetUID()] = entity;
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
	m_gameLoop->Update(deltaTime);

	for (auto iterator = m_entities.begin(); iterator != m_entities.end(); ++iterator)
	{
		auto entity = (*iterator).second;
		entity->Update(deltaTime);
	}
}

void Core::Render()
{
	BeginDrawing();
	ClearBackground(DARKGRAY);

	m_gameLoop->Render();

	for (auto iterator = m_entities.begin(); iterator != m_entities.end(); ++iterator)
	{
		auto entity = (*iterator).second;
		entity->Render();
	}
	
	EndDrawing();
}

