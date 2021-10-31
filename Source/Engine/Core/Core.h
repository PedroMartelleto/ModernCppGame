#pragma once

#include "../Commons.h"
#include "../Render/Window.h"
#include "../Render/TextureManager.h"

class GameCore;
class TileMap;
class TextureAtlas;

struct DebugLineInfo
{
	Vec2f start;
	Vec2f end;
	float thickness;
	Color4f color;
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

	void SetWindowSizeAndCenter(int width, int height);

	void DEBUG_DrawRect(const Vec2f& pos, const Vec2f& size, const Color4f& color);
	void DEBUG_DrawBodyAABB(b2Body* body, const Color4f& color);
	void DEBUG_DrawBody(b2Body* body, const Color4f& color);
	void DEBUG_DrawLine(const Vec2f& start, const Vec2f& end, const Color4f& color = Colors::RED, float thickness = 4.0f);
public:
	GameCore* gameCore;
	Ref<TextureManager> textureManager;
private:
	float m_frameTime;
	bool m_isRunning;
	Window* m_window;

	std::vector<std::pair<Rect2D, Color4f>> m_debugRects;
	std::vector<DebugLineInfo> m_debugLines;
};

