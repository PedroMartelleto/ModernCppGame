#pragma once

#include "../Core/GameObject.h"
#include "TileMapLayer.h"
#include "Tileset.h"

class TextureManager;

class TileMap
{
public:
	TileMap(const std::string& fileName, TextureManager* textureManager);
	~TileMap();

	int GetLayerCount() const;
	unsigned char GetTile(int layer, int x, int y) const;
	std::vector<TileMapLayer*> GetLayers();

	void Destroy();
private:
	std::vector<TileMapLayer*> m_layers;
	Tileset* m_tileset;
};

