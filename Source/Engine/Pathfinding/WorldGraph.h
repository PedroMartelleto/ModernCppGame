#pragma once

#include "../Commons.h"

using WorldNodeID = uint32_t;

struct WorldLink
{
	float cost;
	WorldNodeID dst;
	Color4f color = Colors::WHITE;
};

struct WorldNode
{
	WorldNodeID id;

	bool isBottomEdge;
	bool isHorizontalEdge;

	Vec2f worldPos;

	/// <summary>
	/// Vector of pairs containing cost and node id.
	/// </summary>
	std::vector<WorldLink> links;

	Color4f color = Colors::WHITE;
};

struct WorldGraph
{
	std::map<WorldNodeID, WorldNode> nodes;

	/// <summary>
	/// Calculates the euclidean distance between all nodes and the given position.
	/// Returns the node with the smallest euclidean distance.
	/// </summary>
	/// <param name="anchorPos">Point to calculate distances from.</param>
	/// <returns>The node with the smallest euclidean distance.</returns>
	WorldNode& GetClosestNode(const Vec2f& anchorPos);

	/// <summary>
	/// Validates the links in the graph to see if any invalid children id is found.
	/// In that case, removes that child id.
	/// </summary>
	/// <returns>Returns true if the WorldGraph was valid when this function was called.</returns>
	bool ValidateAndFix();

	/// <summary>
	/// Calculates cost of moving from one node to the next using manhattan distance.
	/// </summary>
	void CalculateManhattanCosts(float horizontalWeight, float verticalWeight, float bottomEdgeCost, float horizontalEdgeCost);

	/// <summary>
	/// Draws this graph on screen.
	/// </summary>
	/// <param name="scale">Scale of the drawing.</param>
	/// <param name="z">Z index.</param>
	void Draw(float scale, int z);

	inline WorldNode& operator[](WorldNodeID id)
	{
		return nodes[id];
	}

	inline WorldNode at(WorldNodeID id) const
	{
		return nodes.at(id);
	}
};