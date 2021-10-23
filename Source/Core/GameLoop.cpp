#include "GameLoop.h"
#include "Core.h"
#include "Player.h"

GameLoop::GameLoop(Core* core)
{
	m_core = core;
	m_player = nullptr;
}

void GameLoop::Create()
{
	m_player = new Player(m_core);
	m_core->AddEntity(m_player);
}

void GameLoop::Update(float deltaTime)
{
}

void GameLoop::Render()
{
}

void GameLoop::Destroy()
{
}
