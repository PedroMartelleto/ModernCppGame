#include "AStarSearch.h"
#include <queue>

namespace AStarSearch
{
	WorldGraphPath FindShortestPath(WorldGraph& graph, WorldNodeID src, WorldNodeID dst, const HeuristicFn& heuristic)
	{
		if (src == dst) return {};

		// Min heap of pairs <priority, node id>.
		// Priority = heuristic + cost
		std::priority_queue<std::pair<float, WorldNodeID>, std::vector<std::pair<float, WorldNodeID>>, std::greater<std::pair<float, WorldNodeID>>> frontier;
		std::unordered_map<WorldNodeID, float> costSoFar;
		WorldGraphPath nextNodes;
		Vec2f dstPos = graph.nodes[dst].worldPos;

		frontier.push({ 0.0f, src });

		// Dijkstra's algorithm + A* goal heuristic
		while (!frontier.empty())
		{
			auto [currDist, currID] = frontier.top();
			frontier.pop();

			if (currID == dst) return nextNodes;

			for (const auto& [cost, childID, _] : graph[currID].links)
			{
				float newCost = currDist + cost;
				if (costSoFar.find(childID) == costSoFar.end() || newCost < costSoFar[childID])
				{
					costSoFar[childID] = newCost;
					nextNodes[currID] = childID;

					float priority = newCost + heuristic(graph.nodes[childID].worldPos, dstPos);
					frontier.push({ priority, childID });
				}
			}
		}

		return nextNodes;
	}
}