#pragma once

#include "../Commons.h"

class CollisionMask
{
public:
	static CollisionMask* FromFile(const std::string& fileName, int tileWidth, int tileHeight);

	CollisionMask(bool* mask, int width, int height, int tileWidth, int tileHeight);
	~CollisionMask();

	bool IsPixelSolid(TileID tileID, int px, int py) const;
public:
	bool* mask;
	const int width;
	const int height;
	const int tileWidth;
	const int tileHeight;
};

