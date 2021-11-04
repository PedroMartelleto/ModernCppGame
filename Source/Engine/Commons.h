#pragma once

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 128

#include <string>
#include <vector>
#include <map>
#include <array>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>
#include "magic_enum.h"
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <SDL2/SDL.h>
#include <gl/glew.h>
#include "Core/Logger.h"
#include "Core/Math.h"
#include "Core/Timer.h"
#include "Core/Utils.h"

typedef unsigned char TileID;

using json = nlohmann::json;

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
inline constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Array = std::vector<T>;

template<typename K, typename V>
using RBTree = std::unordered_map<K, V>;

template<typename K, typename V>
using HashMap = std::map<K, V>;

#include "enet/enet.h"