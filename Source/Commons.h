#pragma once

#include <string>
#include <vector>
#include <map>
#include <array>
#include <iostream>
#include <unordered_map>
#include "Core/Logger.h"
#include "Core/GameObject.h"
#include "Core/GameMath.h"
#include "Core/Timer.h"
#include "Core/Utils.h"
#include "raylib.h"
#include "box2d/box2d.h"
#include "Globals.h"

typedef unsigned char TileID;
typedef uint64_t UID;

template<typename T>
using Array = std::vector<T>;

template<typename K, typename V>
using RBTree = std::unordered_map<K, V>;

template<typename K, typename V>
using HashMap = std::map<K, V>;