#include "AStarSearch.h"
#include <queue>

namespace AStarSearch
{
	WorldGraphPath FindShortestPath(const WorldGraph& graph, WorldNodeID src, WorldNodeID dst, const HeuristicFn& heuristic)
	{
		if (src == dst) return {};

		// Min heap of pairs <priority, node id>.
		// Priority = heuristic + cost
		std::priority_queue<std::pair<float, WorldNodeID>, std::vector<std::pair<float, WorldNodeID>>, std::greater<std::pair<float, WorldNodeID>>> frontier;
		std::unordered_map<WorldNodeID, float> costSoFar;
		WorldGraphPath cameFrom;
		WorldGraphPath path;
		Vec2f dstPos = graph.at(dst).worldPos;

		frontier.push({ 0.0f, src });

		// Dijkstra's algorithm + A* goal heuristic
		while (!frontier.empty())
		{
			auto [_, currID] = frontier.top();
			frontier.pop();

			if (currID == dst) break;

			for (const auto& [cost, childID] : graph.at(currID).links)
			{
				float newCost = costSoFar[currID] + cost;
				if (costSoFar.find(childID) == costSoFar.end() || newCost < costSoFar[childID])
				{
					costSoFar[childID] = newCost;
					cameFrom[childID] = currID;

					float priority = newCost + heuristic(graph.at(childID).worldPos, dstPos);
					frontier.push({ priority, childID });
				}
			}
		}

		int i = graph.nodes.size();
		auto sentinel = dst;

		while (sentinel != src && i > 0)
		{
			auto prevNode = cameFrom[sentinel];
			path[prevNode] = sentinel;

			sentinel = prevNode;
			i -= 1;
		}

		return path;
	}
}