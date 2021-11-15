#include "TileMap.h"
#include "../Render/ResourceManager.h"
#include "../Core/Core.h"
#include "../../Game/Globals.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

// Global constant for now
const std::string tilesetTexturePath = "Resources/Sprites/Desert/Atlas.png";
const int tilesetTileSize = 16;

TileMap::TileMap(b2World& physicsWorld, float mapScale, const std::string& mapXMLData, Ref<ResourceManager> resourceManager) :
	mapScale(mapScale)
{
	tinyxml2::XMLDocument mapXML;
	mapXML.Parse(mapXMLData.c_str(), mapXMLData.size());

	auto root = mapXML.FirstChildElement("map");
	auto tileWidth = tilesetTileSize;
	auto tileHeight = tilesetTileSize;
	auto texture = resourceManager->GetTexture(tilesetTexturePath, false);

	tileset = CreateRef<Tileset>(texture, tileWidth, tileHeight, resourceManager);

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
			for (const auto& tile : lineTiles)
			{
				if (tile.length() > 0)
				{
					uint64_t tileID = std::stoul(tile);
					tiles[x + y * width] = tileID <= 255 ? (TileID)tileID : 0;
					x += 1;
				}
			}
			y += 1;
		}

		layers.push_back(CreateRef<TileMapLayer>(tiles, width, height, std::string(name), tileset));
	}

	auto objectGroup = root->FirstChildElement("objectgroup");

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(0.0f, 0.0f);
	body = physicsWorld.CreateBody(&bodyDef);

	for (auto objGroupElement = root->FirstChildElement("objectgroup"); objGroupElement != NULL; objGroupElement = objGroupElement->NextSiblingElement("objectgroup"))
	{
		if (strcmp(objGroupElement->Attribute("name"), "Collision") == 0)
		{
			auto scale = mapScale * Game::PIXELS_TO_METERS;

			// For each collision object...
			for (auto element = objGroupElement->FirstChildElement("object"); element != NULL; element = element->NextSiblingElement("object"))
			{
				auto x0 = element->FloatAttribute("x");
				auto y0 = element->FloatAttribute("y");

				auto polyElement = element->FirstChildElement("polygon");
				b2PolygonShape shape;

				// If the body is a rectangle...
				if (polyElement == nullptr)
				{
					auto width = element->FloatAttribute("width");
					auto height = element->FloatAttribute("height");
					b2Vec2 vertices[4] = {
						b2Vec2(x0 * scale, y0 * scale),
						b2Vec2((x0 + width) * scale, y0 * scale),
						b2Vec2((x0 + width) * scale, (y0 + height) * scale),
						b2Vec2(x0 * scale, (y0 + height) * scale)
					};
					shape.Set(&vertices[0], 4);
				}
				// Else it is a polygon
				else
				{
					float rotation = element->Attribute("rotation") == NULL ? 0.0f : glm::radians(element->FloatAttribute("rotation"));
					auto rotMatrix = rotation == 0.0f ? Matrix4f(1.0f) : glm::rotate(Matrix4f(1.0f), rotation, Vec3f(0, 0, 1));
					auto rotCenter = Vec2f(1.0f, 1.0f);

					// Adds the polygon points to a box 2d shape
					auto points = std::vector<b2Vec2>();

					for (auto pointStr : Utils::StringSplit(polyElement->Attribute("points"), " "))
					{
						auto coordsStr = Utils::StringSplit(pointStr, ",");
						auto x = std::stof(coordsStr[0]);
						auto y = std::stof(coordsStr[1]);
						
						if (rotation == 0.0f)
						{
							points.push_back(b2Vec2((x0 + x) * scale, (y0 + y) * scale));
						}
						else 
						{
							auto lastPoint = Vec2f(x, y);
							Vec4f rotPos = rotMatrix * Vec4f(x, y, 0.0f, 1.0f);
							points.push_back(b2Vec2((rotPos.x + x0) * scale, (rotPos.y + y0) * scale));
						}
					}

					assert(points.size() <= 8);
					shape.Set(&points[0], points.size());
				}

				body->CreateFixture(&shape, 0.0f);
			}
		}
		else if (strcmp(objGroupElement->Attribute("name"), "Spawn") == 0)
		{
			// For each collision object...
			for (auto element = objGroupElement->FirstChildElement("object"); element != NULL; element = element->NextSiblingElement("object"))
			{
				auto x0 = element->FloatAttribute("x");
				auto y0 = element->FloatAttribute("y");
				spawns.push_back(Vec2f(x0, y0));
			}
		}
	}

	scaledTileSize = Vec2f(((float)tileset->tileWidth) * mapScale, ((float)tileset->tileHeight) * mapScale);
	scaledInvTileSize = Vec2f(1.0f / scaledTileSize.x, 1.0f / scaledTileSize.y);
}

TileMap::~TileMap()
{
	layers.clear();

	if (tileset != nullptr)
	{
		tileset = nullptr;
	}
}

Vec2f TileMap::GetSpawn() const
{
	return GetSpawns(1)[0];
}

std::vector<Vec2f> TileMap::GetSpawns(int count) const
{
	std::vector<Vec2f> out;

	while (count > (int)spawns.size())
	{
		std::sample(
			spawns.begin(),
			spawns.end(),
			std::back_inserter(out),
			spawns.size(),
			std::mt19937{ std::random_device{}() }
		);
		count -= spawns.size();
	}

	if (count > 0)
	{
		std::sample(spawns.begin(), spawns.end(), std::back_inserter(out), count, std::mt19937{ std::random_device{}() });
	}

	for (auto& v : out)
	{
		v *= mapScale;
	}

	return out;
}

unsigned char TileMap::GetTile(int layer, int x, int y) const
{
	if (layer >= (int) layers.size() || layer < 0)
	{
		return 0;
	}

	return layers[layer]->GetTile(x, y);
}