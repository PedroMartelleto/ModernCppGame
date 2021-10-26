#include "GameLoop.h"
#include "Core.h"
#include "../Entity/Mob/Player.h"
#include "../Map/TileMap.h"

GameLoop::GameLoop(Core* core) :
	core(core),
	player(nullptr)
{
}

void GameLoop::Create()
{
	player = new Player(core);
	core->AddEntity(player);
	player->position = Vec2f(456.0f, 256.0f);

	core->map = new TileMap(core, 3.0f, 1, "Resources/Maps/Map1.tmx", "Resources/Sprites/Desert/AtlasMask.png", core->GetTextureManager());
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
