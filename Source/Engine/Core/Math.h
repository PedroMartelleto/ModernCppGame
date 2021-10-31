#pragma once

#include <math.h>
#include <string>
#include "box2d/box2d.h"
#include "glm/glm.hpp"

const float MATH_PI = 3.1415926535897932384626433832795f;
#define ToRadians(x) (float)(((x) * MATH_PI / 180.0f))
#define ToDegrees(x) (float)(((x) * 180.0f / MATH_PI))

using Vec2f = glm::vec2;
using Vec3f = glm::vec3;
using Vec4f = glm::vec4;
using Color4 = glm::vec4;
using Matrix4f = glm::mat4;
using Matrix3f = glm::mat3;

struct Rect2D
{
	Vec2f pos;
	Vec2f size;

	inline Rect2D() : pos(0, 0), size(0, 0) {}
	inline Rect2D(const Vec2f& pos, const Vec2f& size) : pos(pos), size(size) {}
	inline Rect2D(float x, float y, float width, float height) : pos(x, y), size(width, height) {}
};

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
	static const Color4 BLACK = Color4(0, 0, 0, 1);
	static const Color4 WHITE = Color4(1, 1, 1, 1);
	static const Color4 RED = Color4(1, 0, 0, 1);
	static const Color4 GREEN = Color4(0, 1, 0, 1);
	static const Color4 BLUE = Color4(0, 0, 1, 1);
	static const Color4 YELLOW = Color4(1, 1, 0, 1);
	static const Color4 PINK = Color4(1, 0, 1, 1);
	static const Color4 DARKGRAY = Color4(0.663f, 0.663f, 0.663f, 1);
}