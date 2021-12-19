#include "Core.h"
#include "../Render/Render2D.h"
#include "../Render/ResourceManager.h"
#include "../../Game/Globals.h"
#include "../../Game/GameCore.h"
#include <chrono>
#include <thread>

Core::Core(int targetFPS, HostType hostType) :
	m_window(nullptr),
	m_hostType(hostType)
{
	m_frameTime = 1.0f / (float)targetFPS;
	m_isRunning = false;
	gameCore = new GameCore(this, hostType);
}

void Core::Create()
{
	m_window = new Window(800, 600, m_hostType == HostType::SERVER ? "Server" : "Client");
	Input::Create(m_window);
	Render2D::Create(m_window);
	resourceManager = CreateRef<ResourceManager>("Resources/Sprites/");

	gameCore->Create();
}

void Core::SetWindowSizeAndCenter(int width, int height)
{
	m_window->SetWindowSizeAndCenter(width, height);
}

float Core::GetFrameTime() const
{
	return m_frameTime;
}

void Core::Render()
{
	auto viewMatrix = Matrix4f(1.0f);
	Render2D::BeginRender(viewMatrix);
	Render2D::BeginBatch();

	gameCore->Render();

	Render2D::DrawQueued();

	Render2D::EndBatch();
	Render2D::Flush();
}

void Core::Destroy()
{
	resourceManager->DestroyAll();

	gameCore->Destroy();
	delete gameCore;

	Render2D::Destroy();
	delete m_window;
	SDL_Quit();
}

void Core::Run()
{
	double lastTime = Timer::GetTime();
	double frameCounter = 0;
	double unprocessedTime = 0;
	int frames = 0;

	m_isRunning = true;
	
	while (m_isRunning)
	{
		bool render = false;

		// Computes the time at the start of the frame and the passed time since the last frame
		double startTime = Timer::GetTime();
		double passedTime = startTime - lastTime;
		lastTime = startTime;

		unprocessedTime += passedTime;
		frameCounter += passedTime;

		// Displays FPS
		if (frameCounter >= 3.0)
		{
			double totalTime = ((1000.0 * frameCounter) / ((double)frames));
			DEBUG_LOG("CORE", LOG_MSG, "FrameTime = %.2f ms", (float)totalTime);
			frames = 0;
			frameCounter = 0;
		}
		
		// While there is time to process...
		while (unprocessedTime > m_frameTime)
		{
			m_window->Update();

			if (m_window->IsCloseRequested())
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
			m_window->SwapBuffers();
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

void Core::Update(float deltaTime)
{
	gameCore->Update(deltaTime);
}