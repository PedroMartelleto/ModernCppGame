#include "ResourceManager.h"

ResourceManager::ResourceManager(const std::string& basePath)
{
	m_basePath = basePath;
}

Ref<Texture> ResourceManager::GetTexture(const std::string& name, bool appendBasePath)
{
	auto findResult = m_textures.find(name);

	if (findResult == m_textures.end())
	{
		// If the resource was not found, load it
		auto base = appendBasePath ? m_basePath : "";
		auto texture = CreateRef<Texture>((base + name).c_str());
		m_textures[name] = texture;
		return m_textures[name];
	}
	else
	{
		return (*findResult).second;
	}
}

Ref<TextureAtlas> ResourceManager::GetAtlas(const std::string& name, bool appendBasePath)
{
	auto findResult = m_atlas.find(name);

	if (findResult == m_atlas.end())
	{
		// If the resource was not found, load it
		auto base = appendBasePath ? m_basePath : "";
		m_atlas[name] = TextureAtlas::FromPath(this, (base + name).c_str());
		return m_atlas[name];
	}
	else
	{
		return (*findResult).second;
	}
}

ResourceManager::~ResourceManager()
{
	DestroyAll();
}

void ResourceManager::DestroyAll()
{
	m_textures.clear();
}