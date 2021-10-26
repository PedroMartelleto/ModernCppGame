#include "CollisionMask.h"
#include "raylib.h"
#include <assert.h>

CollisionMask* CollisionMask::FromFile(const std::string& fileName, int tileWidth, int tileHeight)
{
	auto image = LoadImage(fileName.c_str());
	assert(image.format == PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
	
	bool* mask = new bool[image.width * image.height];
	unsigned char* imageData = (unsigned char*) image.data;

	memcpy(mask, imageData, image.width * image.height);

	return new CollisionMask(mask, image.width, image.height, tileWidth, tileHeight);
}

CollisionMask::CollisionMask(bool* mask, int width, int height, int tileWidth, int tileHeight) :
	mask(mask),
	width(width),
	height(height),
	tileWidth(tileWidth),
	tileHeight(tileHeight)
{
}

CollisionMask::~CollisionMask()
{
	delete[] mask;
	mask = nullptr;
}

bool CollisionMask::IsPixelSolid(TileID tileID, int px, int py) const
{
	if (px < 0 || py < 0 || px >= tileWidth || py >= tileHeight)
	{
		return false;
	}

	// Computes the number of horizontal and vertical tiles in the tileset
	auto tw = width / tileWidth;
	auto th = height / tileHeight;

	// Converts tile id to a (x, y) pixel position in the tileset
	int tindex = ((int) tileID) - 1;
	int tileX = (tindex % tw) * tileWidth + px;
	int tileY = ((tindex / tw) % th) * tileHeight + py;

	// Returns the mask at the specified pixel offset
	return mask[tileX + tileY * width];
}