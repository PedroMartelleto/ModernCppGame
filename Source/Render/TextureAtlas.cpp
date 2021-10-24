#include "TextureAtlas.h"
#include <fstream>

TextureAtlas* TextureAtlas::FromFile(const std::string& fileName)
{
	TextureAtlas* atlas = new TextureAtlas();
	
	std::ifstream file(fileName);

	if (file.is_open())
	{
		std::string line;

		// For each line in the file...
		while (getline(file, line))
		{
			std::vector<std::string> splitString = Utils::StringSplit(line, " ");
			// Removes empty strings
			for (auto it = splitString.begin(); it != splitString.end(); )
			{
				if ((*it).length() <= 0)
				{
					it = splitString.erase(it);
				}
				else
				{
					++it;
				}
			}

			if (splitString.size() < 5) {
				continue;
			}

			// Gets texture region and frame count
			Rectangle region;
			region.x = (float) std::stoi(splitString[1]);
			region.y = (float) std::stoi(splitString[2]);
			region.width = (float) std::stoi(splitString[3]);
			region.height = (float) std::stoi(splitString[4]);

			int frameCount = splitString.size() == 5 ? 1 : std::stoi(splitString[5]);
			atlas->m_animations[splitString[0]] = std::pair<Rectangle, int>(region, frameCount);
		}
	}
	else
	{
		LOGGER_ERROR("Failed to load TextureAtlas from " + fileName);
	}

	return atlas;
}

Rectangle TextureAtlas::GetRegion(const std::string& name)
{
	return GetAnimFrameRegion(name, 0);
}

Rectangle TextureAtlas::GetAnimFrameRegion(const std::string& animName, int frame)
{
	// Gets the stored data and offsets the rectangle so that it is at the right place in the animation
	auto animData = m_animations[animName];
	Rectangle rect = animData.first;
	int frameCount = animData.second;
	if (frameCount > 0) {
		int frameOffset = frame % frameCount;
		rect.x += rect.width * frameOffset;
	}
	return rect;
}