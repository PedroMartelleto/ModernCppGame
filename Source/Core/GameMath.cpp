#include "GameMath.h"
#include <iomanip>
#include <sstream>

const Vec2f Vec2f::zero = Vec2f(0, 0);
const Vec2f Vec2f::one = Vec2f(1, 1);
const Vec3f Vec3f::zero = Vec3f(0, 0, 0);
const Vec3f Vec3f::one = Vec3f(1, 1, 1);
const Vec2f Vec2f::unitX = Vec2f(1, 0);
const Vec2f Vec2f::unitY = Vec2f(0, 1);

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

Vec2i::Vec2i(const Vec2f& v)
{
	this->x = (int)v.x;
	this->y = (int)v.y;
}

Vec2f::Vec2f(const Vec2f& r)
{
	x = r.x;
	y = r.y;
}

Vec2f::Vec2f(const Vec2i& r)
{
	x = (float) r.x;
	y = (float) r.y;
}

Vec2f::Vec2f(float x, float y)
{
	this->x = x;
	this->y = y;
}

float Vec2f::Cross(const Vec2f& r) const
{
	return x * r.y - y * r.x;
}

std::string Vec2f::ToString() const
{
	std::stringstream stream;
	stream << "(";
	stream << std::fixed << std::setprecision(1) << x;
	stream << ",";
	stream << std::fixed << std::setprecision(1) << y;
	stream << ")";
	return stream.str();
}

std::string Vec3f::ToString() const
{
	std::stringstream stream;
	stream << "(";
	stream << std::fixed << std::setprecision(1) << x;
	stream << ",";
	stream << std::fixed << std::setprecision(1) << y;
	stream << ",";
	stream << std::fixed << std::setprecision(1) << z;
	stream << ")";
	return stream.str();
}