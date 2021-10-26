#pragma once

#include "../Commons.h"
#include "../Render/TextureManager.h"

class Tileset
{
public:
	Tileset(Texture2D texture, int tileWidth, int tileHeight, TextureManager* textureManager);

	Rectangle GetTileRegion(int x, int y) const;
	Rectangle GetTileRegion(TileID id) const;

	Vec2f TileSize() const;
public:
	Texture2D texture;
	const int tileWidth;
	const int tileHeight;
	int tileCountWidth;
	int tileCountHeight;
	TextureManager* textureManager;
};

