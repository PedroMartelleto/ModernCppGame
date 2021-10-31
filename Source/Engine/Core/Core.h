#pragma once

#include "../Commons.h"
#include "../Render/Window.h"

class GameCore;
class TileMap;
class TextureAtlas;
class TextureManager;

struct DebugLineInfo
{
	Vec2f start;
	Vec2f end;
	float thickness;
	Color4 color;
};

class Core
{
public:
	Core(int targetFPS);

	virtual void Create();
	virtual void Destroy();
	virtual void Render();
	virtual void Update(float deltaTime);
	
	void Run();

	/*
	void DEBUG_DrawRect(const Vec2f& pos, const Vec2f& size, const RGBAColor& color);
	void DEBUG_DrawBodyAABB(b2Body* body, const RGBAColor& color);
	void DEBUG_DrawBody(b2Body* body, const RGBAColor& color);
	void DEBUG_DrawLine(const Vec2f& start, const Vec2f& end, const RGBAColor& color = RGBAColor::RED, float thickness = 4.0f);*/
public:
	// GameCore* gameCore;
private:
	float m_frameTime;
	bool m_isRunning;
	Window* m_window;

	// std::vector<std::pair<raylib::Rectangle, RGBAColor>> m_debugRects;
	// std::vector<DebugLineInfo> m_debugLines;
};

