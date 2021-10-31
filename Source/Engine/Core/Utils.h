#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <chrono>

namespace Utils
{
	std::vector<std::string> StringSplit(const std::string& str, const std::string& delimiter);

	std::string GetDirectoryPath(const std::string& filePath);
};

