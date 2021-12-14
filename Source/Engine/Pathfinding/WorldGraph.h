#pragma once

#include "../Commons.h"

using WorldNodeID = uint32_t;

struct WorldLink
{
	float cost;
	WorldNodeID dst;

	WorldLink(float cost, WorldNodeID dst) :
		cost(cost), dst(dst)
	{}
};

struct WorldNode
{
	WorldNodeID id = 0;

	bool isBottomEdge = false;
	bool isHorizontalEdge = false;

	Vec2f worldPos = Vec2f(0.0f, 0.0f);

	std::vector<WorldLink> links;

	WorldNode() {}

	WorldNode(WorldNodeID id, bool isBottomEdge, bool isHorizontalEdge, const Vec2f& worldPos, const std::vector<WorldLink>& links) :
		id(id), isBottomEdge(isBottomEdge), isHorizontalEdge(isHorizontalEdge), worldPos(worldPos), links(links)
	{}
};

struct WorldGraph
{
	/// <summary>
	/// Calculates cost of moving from one node to the next using manhattan distance.
	/// </summary>
	static void CalculateManhattanCosts(std::unordered_map<WorldNodeID, WorldNode>& nodes, float horizontalWeight, float verticalWeight, float bottomEdgeCost, float horizontalEdgeCost);

	std::unordered_map<WorldNodeID, WorldNode> nodes;

	WorldGraph(const std::unordered_map<WorldNodeID, WorldNode>& nodes) : nodes(nodes) {}
	
	/// <summary>
	/// Calculates the euclidean distance between all nodes and the given position.
	/// Returns the node with the smallest euclidean distance.
	/// </summary>
	/// <param name="anchorPos">Point to calculate distances from.</param>
	/// <returns>The node with the smallest euclidean distance.</returns>
	WorldNodeID GetClosestNode(const Vec2f& anchorPos) const;

	/// <summary>
	/// Validates the links in the graph to see if any invalid children id is found.
	/// </summary>
	/// <returns>Returns true if the WorldGraph is valid.</returns>
	bool Validate() const;

	/// <summary>
	/// Draws this graph on screen.
	/// </summary>
	/// <param name="scale">Scale of the drawing.</param>
	/// <param name="z">Z index.</param>
	void Draw(float scale, float z) const;

	inline WorldNode operator[](WorldNodeID id) const
	{
		return nodes.at(id);
	}

	inline WorldNode at(WorldNodeID id) const
	{
		return nodes.at(id);
	}
};