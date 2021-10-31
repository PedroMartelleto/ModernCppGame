#include "Math.h"

std::string Rect2D::ToString() const
{
	return "{ " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(size.x) + ", " + std::to_string(size.y) + " }";
}