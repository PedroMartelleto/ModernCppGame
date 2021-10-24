#include "Tileset.h"

Tileset::Tileset(Texture2D texture, int tileWidth, int tileHeight, TextureManager* textureManager)
{
	m_texture = texture;
	m_tileWidth = tileWidth;
	m_tileHeight = tileHeight;
	m_textureManager = textureManager;
}

TextureManager* Tileset::GetTextureManager()
{
	return m_textureManager;
}

Texture2D Tileset::GetTexture()
{
	return m_texture;
}

Rectangle Tileset::GetTileRegion(int x, int y) const
{
	return CreateRectangle((float)x*m_tileWidth, (float)y*m_tileHeight, (float)m_tileWidth, (float)m_tileHeight);
}

Rectangle Tileset::GetTileRegion(TileID id) const
{
	if (id <= 0)
	{
		return GetTileRegion(0, 0);
	}

	// Converts index to (x,y)
	auto width = m_texture.width / m_tileWidth;
	auto height = m_texture.height / m_tileHeight;
	int tindex = ((int)id) - 1;
	auto x = tindex % width;
	auto y = (tindex / width) % height;
	return GetTileRegion(x, y);
}

int Tileset::GetTileWidth() const
{
	return m_tileWidth;
}

int Tileset::GetTileHeight() const
{
	return m_tileHeight;
}