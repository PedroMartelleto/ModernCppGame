#include "TileMap.h"
#include "tinyxml2.h"
#include "../Render/TextureManager.h"

using namespace tinyxml2;

TileMap::TileMap(const std::string& fileName, TextureManager* textureManager)
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
	auto texture = textureManager->Get(tilesetFolder + pathToImage->Attribute("source"), false);

	m_tileset = new Tileset(texture, tileWidth, tileHeight, textureManager);

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
					tiles[x + y * width] = (TileID)std::stoi(tile);
					x += 1;
				}
			}
			y += 1;
		}

		m_layers.push_back(new TileMapLayer(tiles, width, height, std::string(name), m_tileset));
	}
}

TileMap::~TileMap()
{
	Destroy();
}

int TileMap::GetLayerCount() const
{
	return m_layers.size();
}

unsigned char TileMap::GetTile(int layer, int x, int y) const
{
	if (layer >= m_layers.size() || layer < 0)
	{
		return 0;
	}

	return m_layers[layer]->GetTile(x, y);
}

std::vector<TileMapLayer*> TileMap::GetLayers()
{
	return m_layers;
}

void TileMap::Destroy()
{
	for (auto layer : m_layers)
	{
		layer->Destroy();
		delete layer;
	}

	m_layers.clear();

	if (m_tileset != nullptr)
	{
		delete m_tileset;
		m_tileset = nullptr;
	}
}