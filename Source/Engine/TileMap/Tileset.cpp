#include "Tileset.h"

Tileset::Tileset(Ref<Texture2D> texture, int tileWidth, int tileHeight, const Ref<ResourceManager>& resourceManager) :
	tileWidth(tileWidth),
	tileHeight(tileHeight),
	resourceManager(resourceManager),
	texture(texture),
	tileCountWidth(0),
	tileCountHeight(0) {}

Vec2f Tileset::TileSize() const
{
	return Vec2f((float) tileWidth, (float) tileHeight);
}

Rect2D Tileset::GetTileRegion(int x, int y) const
{
	return Rect2D((float)x*tileWidth, (float)y*tileHeight, (float)tileWidth, (float)tileHeight);
}

Rect2D Tileset::GetTileRegion(TileID id) const
{
	if (id <= 0)
	{
		return GetTileRegion(0, 0);
	}

	// Converts index to (x,y)
	auto width = (int)texture->GetWidth() / tileWidth;
	auto height = (int)texture->GetHeight() / tileHeight;
	int tindex = ((int)id) - 1;
	auto x = tindex % width;
	auto y = (tindex / width) % height;
	return GetTileRegion(x, y);
}