#pragma once

#include "WorldGraph.h"

namespace AStarSearch
{
	using HeuristicFn = std::function<float(Vec2f, Vec2f)>;
	using WorldGraphPath = std::unordered_map<WorldNodeID, WorldNodeID>;

	WorldGraphPath FindShortestPath(WorldGraph& graph, WorldNodeID src, WorldNodeID dest, const HeuristicFn& heuristic);
};