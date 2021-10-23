#pragma once

#include "../Commons.h"

/// <summary>
/// Manages loading/destroying textures.
/// </summary>
class TextureManager
{
public:
	TextureManager(const std::string& basePath);

	Texture2D Get(const std::string& name);
	void Destroy(const std::string& name);
	void DestroyAll();
private:
	std::string m_basePath;
	std::unordered_map<std::string, Texture2D> m_resources;
};

