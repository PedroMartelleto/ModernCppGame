#include "Core.h"
#include "../Texture/TextureAtlas.h"
#include "../../Game/GameCore.h"
#include <chrono>
#include <thread>

Core::Core(int targetFPS)
{
	m_frameTime = 1.0f / (float)targetFPS;
	m_isRunning = false;
	gameCore = new GameCore(this);
}

void Core::Create()
{
	InitWindow(0, 0, "CppGame");

	gameCore->Create();
}

void Core::Destroy()
{
	gameCore->Destroy();
	delete gameCore;

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

void Core::Update(float deltaTime)
{
	gameCore->Update(deltaTime);
}

void Core::Render()
{
	BeginDrawing();
	ClearBackground(DARKGRAY);

	gameCore->Render();

	for (const auto& rect : m_debugRects)
	{
		auto rectToDraw = rect.first;
		DrawRectangle((int) rectToDraw.x, (int) rectToDraw.y, (int) rectToDraw.width, (int) rectToDraw.height, rect.second);
	}

	for (const auto& line : m_debugLines)
	{
		DrawLineEx(line.start.raylib(), line.end.raylib(), line.thickness, line.color);
	}

	m_debugRects.clear();
	
	EndDrawing();
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
			const auto& aabb = fixture->GetAABB(childIndex);
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
			line.start = Vec2f(polygonShape->m_vertices[i - 1]) * METERS_TO_PIXELS;
			line.end = Vec2f(polygonShape->m_vertices[i]) * METERS_TO_PIXELS;
			m_debugLines.push_back(line);
		}
		line.start = Vec2f(polygonShape->m_vertices[polygonShape->m_count - 1]) * METERS_TO_PIXELS;
		line.end = Vec2f(polygonShape->m_vertices[0]) * METERS_TO_PIXELS;
		m_debugLines.push_back(line);
		fixture = fixture->GetNext();
	}
}

void Core::DEBUG_DrawLine(const Vec2f& start, const Vec2f& end, const Color& color, float thickness)
{
	DebugLineInfo dbLineInfo;
	dbLineInfo.start = start;
	dbLineInfo.end = end;
	dbLineInfo.color = color;
	dbLineInfo.thickness = thickness;
	m_debugLines.push_back(dbLineInfo);
}