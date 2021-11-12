#include "Math.h"

std::string Rect2D::ToString() const
{
	return "{ " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(width) + ", " + std::to_string(height) + " }";
}