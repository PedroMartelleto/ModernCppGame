#pragma once

#include "../Commons.h"
#include "../Render/TextureManager.h"

class GameLoop;
class TileMap;
class GameEntity;
class TextureAtlas;
class TextureManager;

struct DebugLineInfo
{
	Vec2f start;
	Vec2f end;
	float thickness;
	Color color;
};

class Core : GameObject
{
public:
	Core(int targetFPS);

	virtual void Create();
	virtual void Destroy();
	virtual void Render();
	virtual void Update(float deltaTime);
	void Run();

	void DEBUG_DrawRect(const Vec2f& pos, const Vec2f& size, const Color& color);
	void DEBUG_DrawBodyAABB(b2Body* body, const Color& color);
	void DEBUG_DrawBody(b2Body* body, const Color& color);

	void SetWindowSizeAndCenter(int windowWidth, int windowHeight) const;

	UID CreateUID();

	void AddEntity(GameEntity* entity);
	GameEntity* GetEntity(UID uid);
	void DestroyEntity(UID uid);

	TextureAtlas* GetTextureAtlas();
	TextureManager* GetTextureManager();
public:
	TileMap* map;
	b2World physicsWorld;
private:
	/// <summary>
	/// Maps an UID to a Game Entity.
	/// Using a std::map here is better since we need iterate in-order.
	/// </summary>
	std::map<UID, GameEntity*> m_entities;

	GameLoop* m_gameLoop;
	TextureAtlas* m_atlas;
	TextureManager* m_textureManager;

	float m_frameTime;
	bool m_isRunning;
	UID m_uidCounter;

	std::vector<std::pair<Rectangle, Color>> m_debugRects;
	std::vector<DebugLineInfo> m_debugLines;
};

