#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "nlohmann/json.hpp"

namespace Utils
{
	std::vector<std::string> StringSplit(const std::string& str, const std::string& delimiter);
	std::string GetDirectoryPath(const std::string& filePath);
	nlohmann::json LoadJSON(const std::string& filePath);
	std::string LoadFile(const std::string& filePath);
};