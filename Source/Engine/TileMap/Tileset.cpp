#include "Tileset.h"

Tileset::Tileset(Texture2D texture, int tileWidth, int tileHeight, TextureManager* textureManager) :
	tileWidth(tileWidth),
	tileHeight(tileHeight),
	textureManager(textureManager),
	texture(texture),
	tileCountWidth(0),
	tileCountHeight(0)
{
}

Vec2f Tileset::TileSize() const
{
	return Vec2f((float) tileWidth, (float) tileHeight);
}

Rectangle Tileset::GetTileRegion(int x, int y) const
{
	return CreateRectangle((float)x*tileWidth, (float)y*tileHeight, (float)tileWidth, (float)tileHeight);
}

Rectangle Tileset::GetTileRegion(TileID id) const
{
	if (id <= 0)
	{
		return GetTileRegion(0, 0);
	}

	// Converts index to (x,y)
	auto width = texture.width / tileWidth;
	auto height = texture.height / tileHeight;
	int tindex = ((int)id) - 1;
	auto x = tindex % width;
	auto y = (tindex / width) % height;
	return GetTileRegion(x, y);
}