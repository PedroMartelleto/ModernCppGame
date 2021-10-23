#pragma once

#include "../Commons.h"
#include "../Render/TextureManager.h"
#include "../Entity/GameEntity.h"

class GameLoop;

class Core : GameObject
{
public:
	Core(int targetFPS);

	virtual void Create();
	virtual void Destroy();
	virtual void Render();
	virtual void Update(float deltaTime);
	void Run();

	UID CreateUID();

	void AddEntity(GameEntity* entity);
	GameEntity* GetEntity(UID uid);
	void DestroyEntity(UID uid);

	TextureAtlas* GetTextureAtlas();
	TextureManager* GetTextureManager();
private:
	/// <summary>
	/// Maps an UID to a Game Entity.
	/// Using a std::map here is better since we need iterate in-order.
	/// </summary>
	std::map<UID, GameEntity*> m_entities;

	int m_windowWidth;
	int m_windowHeight;
	float m_frameTime;
	bool m_isRunning;
	UID m_uidCounter;

	GameLoop* m_gameLoop;
	TextureAtlas* m_atlas;
	TextureManager* m_textureManager;
};

