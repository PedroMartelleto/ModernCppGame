#pragma once

#include "../Commons.h"
#include "../Render/TextureManager.h"

class Tileset
{
public:
	Tileset(Texture2D texture, int tileWidth, int tileHeight, TextureManager* textureManager);

	TextureManager* GetTextureManager();
	Texture2D GetTexture();
	Rectangle GetTileRegion(int x, int y) const;
	Rectangle GetTileRegion(TileID id) const;
	int GetTileWidth() const;
	int GetTileHeight() const;
private:
	Texture2D m_texture;
	int m_tileWidth;
	int m_tileHeight;
	int m_tileCountWidth;
	int m_tileCountHeight;
	TextureManager* m_textureManager;
};

