#include "GameMath.h"
#include <iomanip>
#include <sstream>

const Vec2f Vec2f::zero = Vec2f(0, 0);
const Vec2f Vec2f::one = Vec2f(1, 1);
const Vec3f Vec3f::zero = Vec3f(0, 0, 0);
const Vec3f Vec3f::one = Vec3f(1, 1, 1);

BoundingBox CreateBoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	BoundingBox box;
	raylib_Vec3 min = Vec3f(minX, minY, minZ).raylib();
	raylib_Vec3 max = Vec3f(maxX, maxY, maxZ).raylib();
	box.min = min;
	box.max = max;
	return box;
}

Rectangle CreateRectangle(float x, float y, float width, float height)
{
	Rectangle rect;
	rect.x = x;
	rect.y = y;
	rect.width = width;
	rect.height = height;
	return rect;
}

std::string Vec2f::ToString() const
{
	std::stringstream stream;
	stream << "(";
	stream << std::fixed << std::setprecision(1) << GetX();
	stream << ",";
	stream << std::fixed << std::setprecision(1) << GetY();
	stream << ")";
	return stream.str();
}

std::string Vec3f::ToString() const
{
	std::stringstream stream;
	stream << "(";
	stream << std::fixed << std::setprecision(1) << GetX();
	stream << ",";
	stream << std::fixed << std::setprecision(1) << GetY();
	stream << ",";
	stream << std::fixed << std::setprecision(1) << GetZ();
	stream << ")";
	return stream.str();
}