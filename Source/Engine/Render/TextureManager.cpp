#include "TextureManager.h"

TextureManager::TextureManager(const std::string& basePath)
{
	m_basePath = basePath;
}

Ref<Texture> TextureManager::Get(const std::string& name, bool appendBasePath)
{
	auto findResult = m_resources.find(name);

	if (findResult == m_resources.end())
	{
		// If the resource was not found, load it
		auto base = appendBasePath ? m_basePath : "";
		auto texture = CreateRef<Texture>((base + name).c_str());
		m_resources[name] = texture;
		return m_resources[name];
	}
	else
	{
		return (*findResult).second;
	}
}

TextureManager::~TextureManager()
{
	DestroyAll();
}

void TextureManager::DestroyAll()
{
	m_resources.clear();
}