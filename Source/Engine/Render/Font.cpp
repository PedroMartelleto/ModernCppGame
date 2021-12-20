#include "Font.h"

Font::Font(const std::string& name, const Ref<Texture>& texture, int tileWidth, int tileHeight, const std::vector<char>& characters) :
	name(name),
	tileWidth(tileWidth),
	tileHeight(tileHeight),
	characters(characters)
{
	int tileCountWidth = (int)texture->GetWidth() / tileWidth;
	int tileCountHeight = (int)texture->GetHeight() / tileHeight;

	// Creates a texture atlas with the rectangles for this font
	atlas = CreateRef<TextureAtlas>();
	atlas->texture = texture;

	for (uint32_t i = 0; i < characters.size(); ++i)
	{
		float x = (float)(i % tileCountWidth) * (float) tileWidth;
		float y = (float)(i / tileCountWidth) * (float) tileHeight;

		atlas->m_animations["" + characters[i]] = std::pair<Rect2D, int>(Rect2D(x, y, (float)tileWidth, (float)tileHeight), 1);
	}
}