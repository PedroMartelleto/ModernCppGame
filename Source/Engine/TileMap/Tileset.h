#pragma once

#include "../Commons.h"
#include "../Render/ResourceManager.h"

class Tileset
{
public:
	Tileset(Ref<Texture2D> texture, int tileWidth, int tileHeight, const Ref<ResourceManager>& resourceManager);

	Rect2D GetTileRegion(int x, int y) const;
	Rect2D GetTileRegion(TileID id) const;

	Vec2f TileSize() const;
public:
	Ref<Texture2D> texture;
	const int tileWidth;
	const int tileHeight;
	int tileCountWidth;
	int tileCountHeight;
	Ref<ResourceManager> resourceManager;
};

