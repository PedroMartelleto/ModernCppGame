#include "WorldGraph.h"
#include "../Render/Render2D.h"

bool WorldGraph::Validate() const
{
	bool isValid = true;

	for (const auto& [nodeID, node] : nodes)
	{
		assert(nodeID == node.id);

		// For each children...
		auto it = node.links.begin();

		while (it != node.links.end())
		{
			// Checks if the id is valid...
			if (nodes.find(it->dst) == nodes.end())
			{
				DEBUG_LOG("GRAPH", LOG_WARNING, "Found invalid link in a graph. Destination node does not exist. Link: %u -> %u", node.id, it->dst);

				assert(it->cost >= 0.0f);
				assert(false); // Invalid link found!
				isValid = false;
			}
			else
			{
				it += 1;
			}
		}
	}

	return isValid;
}

void WorldGraph::CalculateManhattanCosts(std::unordered_map<WorldNodeID, WorldNode>& nodes, float horizontalWeight, float verticalWeight, float bottomEdgeCost, float horizontalEdgeCost)
{
	for (auto& [nodeID, node] : nodes)
	{
		const Vec2f& src = node.worldPos;

		// Bottom edges have a fixed cost
		if (node.isBottomEdge)
		{
			assert(node.links.size() == 1);
			node.links[0].cost = bottomEdgeCost;
		}
		// As well as horizontal edges
		else if (node.isHorizontalEdge)
		{
			// Convention: the first child of horizontal edge nodes links to the "other side"
			node.links[0].cost = horizontalEdgeCost;

			auto& child = nodes[node.links[0].dst];
			child.links[0].cost = horizontalEdgeCost;
		}
		else
		{
			for (int i = 0; i < (int) node.links.size(); ++i)
			{
				auto& link = node.links[i];
				const Vec2f& dest = nodes[link.dst].worldPos;
				link.cost = fabsf(src.x - dest.x) * horizontalWeight + fabsf(src.y - dest.y) * verticalWeight;
			}
		}
	}
}

void WorldGraph::Draw(float scale, float z)
{
	Vec2f nodeSize = Vec2f(24, 24);

	for (auto& [nodeID, node] : nodes)
	{
		Vec2f srcPos = node.worldPos * scale;

		Render2D::DrawRect(srcPos - nodeSize/2.0f, 0.0f, nodeSize, z + 10, node.color);
		node.color = Colors::WHITE;

		for (auto& link : node.links)
		{
			Vec2f childPos = nodes.at(link.dst).worldPos * scale;
			Vec2f dir = glm::normalize(childPos - srcPos);
			Render2D::DrawLine(true, srcPos, childPos - dir * 24.0f, z, 2.0f, link.color);
			link.color = Colors::WHITE;
		}
	}
}

WorldNodeID WorldGraph::GetClosestNode(const Vec2f& anchorPos) const
{
	float minDist = FLT_MAX - 1.0f;
	WorldNodeID closest = 0;

	for (const auto& [nodeID, node] : nodes)
	{
		auto diff = node.worldPos - anchorPos;
		auto dist = glm::dot(diff, diff);
		if (dist < minDist)
		{
			minDist = dist;
			closest = nodeID;
		}
	}

	return closest;
}