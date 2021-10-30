#pragma once

#include <math.h>
#include "raylib.h"
#include <string>
#include "box2d/box2d.h"

typedef Vector3 raylib_Vec3;
typedef Vector2 raylib_Vec2;
typedef BoundingBox raylib_BoundingBox;

const float MATH_PI = 3.1415926535897932384626433832795f;
#define ToRadians(x) (float)(((x) * MATH_PI / 180.0f))
#define ToDegrees(x) (float)(((x) * 180.0f / MATH_PI))

BoundingBox CreateBoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

class Vec2f;

Rectangle CreateRectangle(const Vec2f& pos, const Vec2f& size);
Rectangle CreateRectangle(float x, float y, float width, float height);

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

class Vec2i
{
public:
	int x;
	int y;

	Vec2i() : x(0), y(0) { }

	Vec2i(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	Vec2i(const Vec2i& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	Vec2i(const Vec2f& v);

	inline Vec2i Multiply(const Vec2i& other) const
	{
		return Vec2i(x * other.x, y * other.y);
	}

	inline Vec2i Divide(const Vec2i& other) const
	{
		return Vec2i(x / other.x, y / other.y);
	}

	inline bool operator==(const Vec2i& other) const { return x == other.x && y == other.y; }
};

class Vec2f
{
public:
	static const Vec2f zero;
	static const Vec2f one;
	static const Vec2f unitX;
	static const Vec2f unitY;

	float x;
	float y;

	Vec2f() : x(0), y(0) { }

	Vec2f(const Vec2i& r);
	
	inline Vec2f(const Vec2f& r)
	{
		x = r.x;
		y = r.y;
	}

	inline Vec2f(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	inline Vec2f(const b2Vec2& v)
	{
		x = v.x;
		y = v.y;
	}

	float Cross(const Vec2f& r) const;

	inline Vector2 raylib() const
	{
		Vector2 vec;
		vec.x = (float)x;
		vec.y = (float)y;
		return vec;
	}

	inline b2Vec2 b2()
	{
		return b2Vec2(x, y);
	}

	inline float Length() const { return sqrtf(x * x + y * y); }
	inline float LengthSquared() const { return x * x + y * y; }
	inline float Dot(const Vec2f& v) const { return x * v.x + y * v.y; }

	inline Vec2f operator+(const Vec2f& r) const { return Vec2f(x + r.x, y + r.y); }
	inline Vec2f operator-(const Vec2f& r) const { return Vec2f(x - r.x, y - r.y); }
	inline Vec2f operator*(float f) const { return Vec2f(x * f, y * f); }
	inline Vec2f operator/(float f) const { return Vec2f(x / f, y / f); }

	inline bool operator==(const Vec2f& r) const { return x == r.x && y == r.y; }
	inline bool operator!=(const Vec2f& r) const { return !operator==(r); }

	inline Vec2f& operator+=(const Vec2f& r)
	{
		this->x += r.x;
		this->y += r.y;

		return *this;
	}

	inline Vec2f& operator-=(const Vec2f& r)
	{
		this->x -= r.x;
		this->y -= r.y;

		return *this;
	}

	inline Vec2f& operator*=(float f)
	{
		this->x *= f;
		this->y *= f;

		return *this;
	}

	inline Vec2f& operator/=(float f)
	{
		this->x /= f;
		this->y /= f;

		return *this;
	}

	inline Vec2f Abs() const
	{
		return Vec2f(fabsf(x), fabsf(y));
	}

	inline Vec2f Multiply(const Vec2f& other) const
	{
		return Vec2f(x * other.x, y * other.y);
	}

	inline Vec2f Divide(const Vec2f& other) const
	{
		return Vec2f(x / other.x, y / other.y);
	}

	inline Vec2f Floor() const
	{
		return Vec2f(floorf(x), floorf(y));
	}

	inline Vec2f Ceil() const
	{
		return Vec2f(ceilf(x), ceilf(y));
	}

	inline Vec2i Floori() const
	{
		return Vec2i((int) floorf(x), (int)floorf(y));
	}

	inline Vec2i Ceili() const
	{
		return Vec2i((int) ceilf(x), (int) ceilf(y));
	}

	std::string ToString() const;
};

class Vec3f
{
public:
	static const Vec3f zero;
	static const Vec3f one;

	float x;
	float y;
	float z;

	Vec3f() : x(0), y(0), z(0) {}

	inline Vec3f(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	inline Vec3f(const Vec3f& r)
	{
		this->x = r.x;
		this->y = r.y;
		this->z = r.z;
	}

	inline Vec3f(const b2Vec3& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}

	inline Vec3f Multiply(const Vec3f& other) const
	{
		return Vec3f(x * other.x, y * other.y, z * other.z);
	}

	inline Vec3f Divide(const Vec3f& other) const
	{
		return Vec3f(x / other.x, y / other.y, z / other.z);
	}

	inline float Length() const { return sqrtf(x * x + y * y + z * z); }
	inline float LengthSquared() const { return x * x + y * y + z * z; }
	inline float Dot(const Vec3f& v) const { return x * v.x + y * v.y + z * v.z; }

	inline Vec2f xy() const { return Vec2f(x, y); }
	inline Vec2f yz() const { return Vec2f(y, z); }
	inline Vec2f zx() const { return Vec2f(z, x); }

	inline Vec2f yx() const { return Vec2f(y, x); }
	inline Vec2f zy() const { return Vec2f(z, y); }
	inline Vec2f xz() const { return Vec2f(x, z); }

	inline Vec3f Cross(const Vec3f& v) const
	{
		const float _x = x * z - z * y;
		const float _y = z * x - x * z;
		const float _z = x * y - y * x;

		return Vec3f(_x, _y, _z);
	}

	inline Vec3f Normalized() const
	{
		const float length = Length();
		return *this / length;
	}

	inline Vec3f operator+(const Vec3f& r) const { return Vec3f(x + r.x, y + r.y, z + r.z); }
	inline Vec3f operator-(const Vec3f& r) const { return Vec3f(x - r.x, y - r.y, z - r.z); }
	inline Vec3f operator*(float f) const { return Vec3f(x * f, y * f, z * f); }
	inline Vec3f operator/(float f) const { return Vec3f(x / f, y / f, z / f); }

	inline bool operator==(const Vec3f& r) const { return x == r.x && y == r.y && z == r.z; }
	inline bool operator!=(const Vec3f& r) const { return !operator==(r); }

	inline Vec3f& operator+=(const Vec3f& r)
	{
		this->x += r.x;
		this->y += r.y;
		this->z += r.z;

		return *this;
	}

	inline Vec3f& operator-=(const Vec3f& r)
	{
		this->x -= r.x;
		this->y -= r.y;
		this->z -= r.z;

		return *this;
	}

	inline Vec3f& operator*=(float f)
	{
		this->x *= f;
		this->y *= f;
		this->z *= f;

		return *this;
	}

	inline Vec3f& operator/=(float f)
	{
		this->x /= f;
		this->y /= f;
		this->z /= f;

		return *this;
	}

	inline Vector3 raylib()
	{
		Vector3 vec;
		vec.x = x;
		vec.y = y;
		vec.z = z;
		return vec;
	}

	inline b2Vec3 b2()
	{
		return b2Vec3(x, y, z);
	}

	inline Vec3f Floor() const
	{
		return Vec3f(floorf(x), floorf(y), floorf(z));
	}

	inline Vec3f Ceil() const
	{
		return Vec3f(ceilf(x), ceilf(y), ceilf(z));
	}

	std::string ToString() const;
};