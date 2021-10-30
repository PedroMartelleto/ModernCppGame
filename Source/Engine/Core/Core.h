#pragma once

#include "../../Commons.h"
#include "../Texture/TextureManager.h"

class GameCore;
class TileMap;
class TextureAtlas;
class TextureManager;

struct DebugLineInfo
{
	Vec2f start;
	Vec2f end;
	float thickness;
	Color color;
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
	void SetWindowSizeAndCenter(int windowWidth, int windowHeight) const;

	void DEBUG_DrawRect(const Vec2f& pos, const Vec2f& size, const Color& color);
	void DEBUG_DrawBodyAABB(b2Body* body, const Color& color);
	void DEBUG_DrawBody(b2Body* body, const Color& color);
	void DEBUG_DrawLine(const Vec2f& start, const Vec2f& end, const Color& color = RED, float thickness = 4.0f);
public:
	GameCore* gameCore;
private:
	float m_frameTime;
	bool m_isRunning;

	std::vector<std::pair<Rectangle, Color>> m_debugRects;
	std::vector<DebugLineInfo> m_debugLines;
};

