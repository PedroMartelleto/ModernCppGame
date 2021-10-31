#pragma once

#include "../Commons.h"

class TextureAtlas
{
public:
	static TextureAtlas* FromFile(const std::string& fileName);

	Rect2D GetRegion(const std::string& name);
	Rect2D GetAnimFrameRegion(const std::string& animName, int frame);
private:	
	/// <summary>
	/// Maps an animation name to a raylib Rectangle and a frame count.
	/// </summary>
	std::unordered_map<std::string, std::pair<Rect2D, int>> m_animations;
};

