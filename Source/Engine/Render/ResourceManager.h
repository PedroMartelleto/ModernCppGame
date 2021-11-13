#pragma once

#include "../Commons.h"
#include "Texture.h"
#include "TextureAtlas.h"

/// <summary>
/// Manages loading/destroying textures.
/// </summary>
class ResourceManager
{
public:
	ResourceManager(const std::string& basePath);
	~ResourceManager();

	void DestroyAll();

	Ref<Texture> GetTexture(const std::string& name, bool appendBasePath);
	Ref<TextureAtlas> GetAtlas(const std::string& name, bool appendBasePath);
private:
	std::string m_basePath;
	std::unordered_map<std::string, Ref<Texture>> m_textures;
	std::unordered_map<std::string, Ref<TextureAtlas>> m_atlas;
};

