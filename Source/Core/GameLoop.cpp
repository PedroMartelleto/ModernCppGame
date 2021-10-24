#include "GameLoop.h"
#include "Core.h"
#include "../Entity/Mob/Player.h"
#include "../Map/TileMap.h"

GameLoop::GameLoop(Core* core)
{
	m_core = core;
	m_player = nullptr;
}

TileMap* map;

void GameLoop::Create()
{
	m_player = new Player(m_core);
	m_core->AddEntity(m_player);

	map = new TileMap("Resources/Maps/Map1.tmx", m_core->GetTextureManager());
}

void GameLoop::Update(float deltaTime)
{
}

void GameLoop::Render()
{
	for (auto layer : map->GetLayers())
	{
		layer->Render(Vec2f::zero, 3.0f);
	}
}

void GameLoop::Destroy()
{
	map->Destroy();
	delete map;
}
