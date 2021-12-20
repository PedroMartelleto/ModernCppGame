#pragma once

#include "../Commons.h"
#include "TextureAtlas.h"

class Font
{
public:
	std::string name;
	int tileWidth;
	int tileHeight;
	std::vector<char> characters;
	Ref<TextureAtlas> atlas;
	
	Font(const std::string& name, const Ref<Texture>& texture, int tileWidth, int tileHeight, const std::vector<char>& characters);

	inline bool HasChar(char ch) const
	{
		return atlas->m_animations.find(ch + "") != atlas->m_animations.end();
	}
};

