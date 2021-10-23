#include "TextureManager.h"

TextureManager::TextureManager(const std::string& basePath)
{
	m_basePath = basePath;
}

Texture2D TextureManager::Get(const std::string& name)
{
	auto findResult = m_resources.find(name);
	if (findResult == m_resources.end())
	{
		// If the resource was not found, load it
		auto texture = LoadTexture((m_basePath + name).c_str());
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