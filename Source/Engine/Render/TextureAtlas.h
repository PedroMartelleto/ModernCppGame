#pragma once

#include "../Commons.h"
#include "Texture.h"

class ResourceManager;

class TextureAtlas
{
public:
	static Ref<TextureAtlas> FromFile(Ref<Texture> texture, const std::string& fileName);
	static Ref<TextureAtlas> FromPath(ResourceManager* manager, const std::string& fileNameWithoutExt);

	Ref<Texture> texture = nullptr;

	TextureAtlas() {}

	Rect2D GetRegion(const std::string& name);
	Rect2D GetAnimFrameRegion(const std::string& animName, int frame);
private:
	/// <summary>
	/// Maps an animation name to a raylib Rectangle and a frame count.
	/// </summary>
	std::unordered_map<std::string, std::pair<Rect2D, int>> m_animations;
};

