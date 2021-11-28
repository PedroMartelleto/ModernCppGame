#include "WorldGraph.h"
#include "../Render/Render2D.h"

bool WorldGraph::ValidateAndFix()
{
	bool isValid = true;

	for (auto& [nodeID, node] : nodes)
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

				// If not, erase connection from the graph
				isValid = false;
				it = node.links.erase(it);
			}
			else
			{
				it += 1;
			}
		}
	}

	return isValid;
}

void WorldGraph::CalculateManhattanCosts(float horizontalWeight, float verticalWeight, float bottomEdgeCost, float horizontalEdgeCost)
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

void WorldGraph::Draw(float scale, int z)
{
	Vec2f nodeSize = Vec2f(24, 24);

	for (auto& [nodeID, node] : nodes)
	{
		Vec2f srcPos = node.worldPos * scale;

		Render2D::DrawRect(srcPos - nodeSize/2.0f, 0.0f, nodeSize, z + 10, node.isMarked ? Colors::RED : Color4f(0.8f, 0.5f, 0.55f, 1.0f));
		node.isMarked = false;

		for (auto& link : node.links)
		{
			Vec2f childPos = nodes.at(link.dst).worldPos * scale;
			Vec2f dir = glm::normalize(childPos - srcPos);
			Render2D::DrawLine(true, srcPos, childPos - dir * 24.0f, z, 2.0f, link.isMarked ? Colors::GREEN : Color4f(0.7f, 0.6f, 0.8f, 1.0f));
			link.isMarked = false;
		}
	}
}

WorldNode& WorldGraph::GetClosestNode(const Vec2f& anchorPos)
{
	float minDist = FLT_MAX - 1.0f;
	WorldNodeID closest = 0;

	for (auto& [nodeID, node] : nodes)
	{
		auto diff = node.worldPos - anchorPos;
		auto dist = glm::dot(diff, diff);
		if (dist < minDist)
		{
			minDist = dist;
			closest = nodeID;
		}
	}

	return nodes[closest];
}