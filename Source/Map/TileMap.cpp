#include "TileMap.h"
#include "tinyxml2.h"
#include "../Render/TextureManager.h"
#include "../Core/Core.h"
#include "../Physics/CollisionMask.h"

using namespace tinyxml2;

TileMap::TileMap(Core* core, float mapScale, int collisionLayerIndex, const std::string& fileName, const std::string& maskPath, TextureManager* textureManager) :
	collisionLayerIndex(collisionLayerIndex),
	core(core),
	mapScale(mapScale)
{
	auto workingDir = std::string(GetDirectoryPath(fileName.c_str())) + "/";

	// Loads main XML
	XMLDocument mainXML;
	mainXML.LoadFile(fileName.c_str());

	auto root = mainXML.FirstChildElement("map");

	// Loads tileset file
	XMLDocument tilesetXML;
	auto tilesetPath = workingDir + root->FirstChildElement("tileset")->Attribute("source");
	tilesetXML.LoadFile(tilesetPath.c_str());
	auto tilesetFolder = std::string(GetDirectoryPath(tilesetPath.c_str())) + "/";

	auto tilesetElement = tilesetXML.FirstChildElement("tileset");
	auto pathToImage = tilesetElement->FirstChildElement("image");
	auto tileWidth = tilesetElement->IntAttribute("tilewidth");
	auto tileHeight = tilesetElement->IntAttribute("tileheight");
	auto texturePath = tilesetFolder + pathToImage->Attribute("source");
	auto texture = textureManager->Get(texturePath, false);

	tileset = new Tileset(texture, tileWidth, tileHeight, textureManager);
	collisionMask = CollisionMask::FromFile(maskPath, tileWidth, tileHeight);

	// For each layer in the map...
	for (auto element = root->FirstChildElement("layer"); element != NULL; element = element->NextSiblingElement("layer"))
	{
		auto width = element->IntAttribute("width");
		auto height = element->IntAttribute("height");
		auto name = element->Attribute("height");
		TileID* tiles = new TileID[width * height];
		auto contents = element->FirstChildElement("data")->GetText();

		// Loads a small CSV containing the tiles
		auto lines = Utils::StringSplit(contents, "\n");
		int y = 0, x = 0;
		for (auto line : lines)
		{
			if (line.length() <= 0) continue;

			auto lineTiles = Utils::StringSplit(line, ",");
			x = 0;
			for (auto tile : lineTiles)
			{
				if (tile.length() > 0)
				{
					tiles[x + y * width] = (TileID) std::stoi(tile);
					x += 1;
				}
			}
			y += 1;
		}

		layers.push_back(new TileMapLayer(tiles, width, height, std::string(name), tileset));
	}

	scaledTileSize = Vec2f(((float)tileset->tileWidth) * mapScale, ((float)tileset->tileHeight) * mapScale);
	scaledInvTileSize = Vec2f(1.0f / scaledTileSize.x, 1.0f / scaledTileSize.y);
}

MapCoords TileMap::WorldToMapPos(const Vec2f& worldPos) const
{
	auto tilePos = worldPos.Multiply(scaledInvTileSize).Floor();
	auto inTilePos = (worldPos - tilePos.Multiply(scaledTileSize)) / mapScale;
	return MapCoords(Vec2i(tilePos), Vec2i(inTilePos));
}

TileMap::~TileMap()
{
	Destroy();
}

bool TileMap::IsPixelSolid(const MapCoords& mapCoords) const
{
	TileID tileID = layers[collisionLayerIndex]->GetTile((int) mapCoords.tilePos.x, (int) mapCoords.tilePos.y);

	if (tileID > 0)
	{
		return collisionMask->IsPixelSolid(tileID, (int) mapCoords.inTileOffset.x, (int) mapCoords.inTileOffset.y);
	}
	else
	{
		return false;
	}
}

std::array<MapCoords, 4> TileMap::GetRectCorners(const Vec2f& pos, const Vec2f& size) const
{
	auto tlCoords = WorldToMapPos(pos);
	auto trCoords = WorldToMapPos(pos + Vec2f(size.x, 0.0f));
	auto blCoords = WorldToMapPos(pos + Vec2f(0, size.y));
	auto brCoords = WorldToMapPos(pos + size);
	return { { tlCoords, trCoords, blCoords, brCoords } };
}

std::array<bool, 4> TileMap::GetOverlaps(const std::array<MapCoords, 4>& corners) const
{
	return {{ IsPixelSolid(corners[0]), IsPixelSolid(corners[1]), IsPixelSolid(corners[2]), IsPixelSolid(corners[3]) }};
}

unsigned char TileMap::GetTile(int layer, int x, int y) const
{
	if (layer >= layers.size() || layer < 0)
	{
		return 0;
	}

	return layers[layer]->GetTile(x, y);
}

void TileMap::Destroy()
{
	for (auto layer : layers)
	{
		layer->Destroy();
		delete layer;
	}

	layers.clear();

	if (tileset != nullptr)
	{
		delete tileset;
		tileset = nullptr;
	}

	if (collisionMask != nullptr)
	{
		delete collisionMask;
		collisionMask = nullptr;
	}
}