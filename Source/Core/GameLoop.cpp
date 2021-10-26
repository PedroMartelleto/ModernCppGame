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

	core->map = new TileMap(core, MAP_SCALE, "Resources/Maps/Map1.tmx", core->GetTextureManager());

	auto mapWidth = core->map->layers[0]->GetWidth();
	auto mapHeight = core->map->layers[0]->GetHeight();

	core->SetWindowSizeAndCenter(mapWidth * MAP_SCALE * TILE_SIZE, mapHeight * MAP_SCALE * TILE_SIZE);
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
