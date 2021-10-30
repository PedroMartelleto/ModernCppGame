#include "TextureManager.h"

TextureManager::TextureManager(const std::string& basePath)
{
	m_basePath = basePath;
}

const Texture2D& TextureManager::Get(const std::string& name, bool appendBasePath)
{
	auto findResult = m_resources.find(name);

	if (findResult == m_resources.end())
	{
		// If the resource was not found, load it
		auto base = appendBasePath ? m_basePath : "";
		auto texture = LoadTexture((base + name).c_str());
		std::cout << m_basePath + name << std::endl;
		m_resources[name] = texture;
		SetTextureFilter(texture, FILTER_POINT);
		return texture;
	}
	else
	{
		return (*findResult).second;
	}
}

void TextureManager::Destroy(const std::string& name)
{
	auto findResult = m_resources.find(name);
	if (findResult != m_resources.end())
	{
		UnloadTexture((*findResult).second);
		m_resources.erase(findResult);
	}
}

void TextureManager::DestroyAll()
{
	for (auto iterator = m_resources.begin(); iterator != m_resources.end(); iterator++)
	{
		UnloadTexture((*iterator).second);
	}

	m_resources.clear();
}