#include "Core.h"
#include "../Render/Render2D.h"
#include "../Render/TextureManager.h"
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
	textureManager = CreateRef<TextureManager>("Resources/Sprites/");

	gameCore->Create();
}

void Core::SetWindowSizeAndCenter(int width, int height)
{
	auto* window = m_window->GetSDLWindow();
	SDL_SetWindowSize(window, width, height);
	Render2D::Resize((float)width, (float)height);

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	SDL_SetWindowPosition(window, (displayMode.w - width) / 2, (displayMode.h - height) / 2);
}

void Core::Render()
{
	auto viewMatrix = Matrix4f(1.0f);
	Render2D::BeginRender(viewMatrix);
	Render2D::BeginBatch();

	gameCore->Render();

	for (const auto& rectTuple : m_debugRects)
	{
		auto [ rect, color, z ] = rectTuple;
		Render2D::DrawRect(rect.pos(), 0.0f, rect.size(), (int)z, color);
	}

	//for (const auto& line : m_debugLines)
	//{
	//	Render2D::DrawLine(line.start, line.end, line.thickness, line.color);
	//}

	m_debugRects.clear();
	m_debugLines.clear();

	Render2D::EndBatch();
	Render2D::Flush();
}

void Core::Destroy()
{
	textureManager->DestroyAll();

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

void Core::DEBUG_DrawRect(const Vec2f& pos, const Vec2f& size, const Color4f& color, float z)
{
	m_debugRects.push_back(std::tuple<Rect2D,Color4f,float>(Rect2D(pos, size), color, z));
}

void Core::DEBUG_DrawBodyAABB(b2Body* body, const Color4f& color)
{
	b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		for (int childIndex = 0; childIndex < fixture->GetShape()->GetChildCount(); ++childIndex)
		{
			const auto& aabb = fixture->GetAABB(childIndex);
			auto bottomRight = Vec2fFromB2(aabb.upperBound) * Game::METERS_TO_PIXELS;
			auto topLeft = Vec2fFromB2(aabb.lowerBound) * Game::METERS_TO_PIXELS;

			m_debugRects.push_back({ Rect2D(topLeft, glm::abs(bottomRight - topLeft)), fixture->IsSensor() ? Colors::RED : color, fixture->IsSensor() ? 10000.0f : 1000.0f });
		}
		fixture = fixture->GetNext();
	}
}

void Core::DEBUG_DrawBody(b2Body* body, const Color4f& color)
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
			line.start = Vec2fFromB2(polygonShape->m_vertices[i - 1]) * Game::METERS_TO_PIXELS;
			line.end = Vec2fFromB2(polygonShape->m_vertices[i]) * Game::METERS_TO_PIXELS;
			m_debugLines.push_back(line);
		}
		line.start = Vec2fFromB2(polygonShape->m_vertices[polygonShape->m_count - 1]) * Game::METERS_TO_PIXELS;
		line.end = Vec2fFromB2(polygonShape->m_vertices[0]) * Game::METERS_TO_PIXELS;
		m_debugLines.push_back(line);
		fixture = fixture->GetNext();
	}
}

void Core::DEBUG_DrawLine(const Vec2f& start, const Vec2f& end, const Color4f& color, float thickness)
{
	DebugLineInfo dbLineInfo;
	dbLineInfo.start = start;
	dbLineInfo.end = end;
	dbLineInfo.color = color;
	dbLineInfo.thickness = thickness;
	m_debugLines.push_back(dbLineInfo);
}