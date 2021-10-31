#pragma once

#include "../Commons.h"
#include "Texture.h"

/// <summary>
/// Manages loading/destroying textures.
/// </summary>
class TextureManager
{
public:
	TextureManager(const std::string& basePath);
	~TextureManager();

	void DestroyAll();

	Ref<Texture> Get(const std::string& name, bool appendBasePath = true);
private:
	std::string m_basePath;
	std::unordered_map<std::string, Ref<Texture>> m_resources;
};

