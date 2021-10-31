#pragma once

#include "../../Commons.h"
#include "../Texture/TextureManager.h"

class Tileset
{
public:
	Tileset(const Texture2D& texture, int tileWidth, int tileHeight, TextureManager* textureManager);

	Rect2D GetTileRegion(int x, int y) const;
	Rect2D GetTileRegion(TileID id) const;

	Vec2f TileSize() const;
public:
	Texture2D texture;
	const int tileWidth;
	const int tileHeight;
	int tileCountWidth;
	int tileCountHeight;
	TextureManager* textureManager;
};

