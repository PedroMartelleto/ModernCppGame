#pragma once

#include <math.h>
#include <string>
#include <nlohmann/json.hpp>
#include "box2d/box2d.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

const float MATH_PI = 3.1415926535897932384626433832795f;
#define ToRadians(x) (float)(((x) * MATH_PI / 180.0f))
#define ToDegrees(x) (float)(((x) * 180.0f / MATH_PI))

using Vec2f = glm::vec2;
using Vec3f = glm::vec3;
using Vec4f = glm::vec4;
using Color4f = glm::vec4;
using Matrix4f = glm::mat4;
using Matrix3f = glm::mat3;

struct Rect2D
{
	float x;
	float y;
	float width;
	float height;

	inline Rect2D() : x(0), y(0), width(0), height(0) {}
	inline Rect2D(const Vec2f& pos, const Vec2f& size) : x(pos.x), y(pos.y), width(size.x), height(size.y) {}
	inline Rect2D(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}
	inline Rect2D(const Rect2D& other) : x(other.x), y(other.y), width(other.width), height(other.height) {}

	std::string ToString() const;

	inline Vec2f pos() const { return Vec2f(x, y); }
	inline Vec2f size() const { return Vec2f(width, height); }
};

inline Vec2f Vec2fFromB2(const b2Vec2& other)
{
	return Vec2f(other.x, other.y);
}

inline b2Vec2 Vec2fToB2(const Vec2f& other)
{
	return b2Vec2(other.x, other.y);
}

namespace Math
{
	template<typename T>
	inline T Clamp(const T& a, const T& min, const T& max)
	{
		if (a < min) return min;
		else if (a > max) return max;
		else return a;
	}

	inline int Max(int a, int b)
	{
		return a < b ? b : a;
	}
};

namespace Colors
{
	static const Color4f BLACK = Color4f(0, 0, 0, 1);
	static const Color4f WHITE = Color4f(1, 1, 1, 1);
	static const Color4f RED = Color4f(1, 0, 0, 1);
	static const Color4f GREEN = Color4f(0, 1, 0, 1);
	static const Color4f BLUE = Color4f(0, 0, 1, 1);
	static const Color4f YELLOW = Color4f(1, 1, 0, 1);
	static const Color4f PINK = Color4f(1, 0, 1, 1);
	static const Color4f DARKGRAY = Color4f(0.663f, 0.663f, 0.663f, 1);

	static Color4f Alpha(const Color4f& color, float alpha)
	{
		return Color4f(color.x, color.y, color.z, alpha);
	}
}

namespace Serialization
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(b2Vec2, x, y)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(b2Vec3, x, y, z)

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vec2f, x, y)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vec3f, x, y, z)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Color4f, x, y, y, z)

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Rect2D, x, y, width, height)
}