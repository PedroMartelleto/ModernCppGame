#pragma once

#include <string>
#include <vector>
#include <map>
#include <array>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "Engine/Core/Logger.h"
#include "Engine/Core/Math.h"
#include "Engine/Core/Timer.h"
#include "Engine/Core/Utils.h"
#include "raylib.h"
#include "box2d/box2d.h"
#include "entt/entt.hpp"
#include "Globals.h"
#include "nlohmann/json.hpp"
#include "enet/enet.h"

typedef unsigned char TileID;

using json = nlohmann::json;

template<typename T>
using Array = std::vector<T>;

template<typename K, typename V>
using RBTree = std::unordered_map<K, V>;

template<typename K, typename V>
using HashMap = std::map<K, V>;