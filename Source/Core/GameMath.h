#pragma once

#include <math.h>
#include "raylib.h"
#include <string>

typedef Vector3 raylib_Vec3;
typedef Vector2 raylib_Vec2;
typedef BoundingBox raylib_BoundingBox;

#define MATH_PI 3.1415926535897932384626433832795
#define ToRadians(x) (float)(((x) * MATH_PI / 180.0f))
#define ToDegrees(x) (float)(((x) * 180.0f / MATH_PI))

BoundingBox CreateBoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

Rectangle CreateRectangle(float x, float y, float width, float height);

namespace GameMath
{
	template<typename T>
	inline T Clamp(const T& a, const T& min, const T& max)
	{
		if (a < min) return min;
		else if (a > max) return max;
		else return a;
	}
};

/*
 * Below, a simple 3D vector math library based on the code by Benny Bobaganoosh.
 * Github link: https://github.com/BennyQBD/3DEngineCpp/blob/master/src/core/math3d.cpp.
 */

template<typename T, unsigned int D>
class Vec
{
public:
	Vec() { }

	inline T Dot(const Vec<T, D>& r) const
	{
		T result = T(0);
		for (unsigned int i = 0; i < D; i++)
			result += (*this)[i] * r[i];

		return result;
	}

	inline Vec<T, D> Max(const Vec<T, D>& r) const
	{
		Vec<T, D> result;
		for (unsigned int i = 0; i < D; i++)
		{
			result[i] = values[i] > r[i] ? values[i] : r[i];
		}

		return result;
	}

	inline T Max() const
	{
		T maxVal = (*this)[0];

		for (int i = 0; i < D; i++)
			if ((*this)[i] > maxVal)
				maxVal = (*this)[i];

		return maxVal;
	}

	inline T LengthSq() const { return this->Dot(*this); }
	inline T Length() const { return sqrt(LengthSq()); }
	inline Vec<T, D> Normalized() const { return *this / Length(); }
	inline Vec<T, D> Lerp(const Vec<T, D>& r, T lerpFactor) const { return (r - *this) * lerpFactor + *this; }

	inline Vec<T, D> Reflect(const Vec<T, D>& normal) const
	{
		return *this - (normal * (this->Dot(normal) * 2));
	}

	inline Vec<T, D> operator+(const Vec<T, D>& r) const
	{
		Vec<T, D> result;
		for (unsigned int i = 0; i < D; i++)
			result[i] = values[i] + r[i];

		return result;
	}

	inline Vec<T, D> operator-(const Vec<T, D>& r) const
	{
		Vec<T, D> result;
		for (unsigned int i = 0; i < D; i++)
			result[i] = values[i] - r[i];

		return result;
	}

	inline Vec<T, D> operator*(const T& r) const
	{
		Vec<T, D> result;
		for (unsigned int i = 0; i < D; i++)
			result[i] = values[i] * r;

		return result;
	}

	inline Vec<T, D> operator/(const T& r) const
	{
		Vec<T, D> result;
		for (unsigned int i = 0; i < D; i++)
			result[i] = values[i] / r;

		return result;
	}

	inline Vec<T, D>& operator+=(const Vec<T, D>& r)
	{
		for (unsigned int i = 0; i < D; i++)
			(*this)[i] = values[i] + r[i];

		return *this;
	}

	inline Vec<T, D>& operator-=(const Vec<T, D>& r)
	{
		for (unsigned int i = 0; i < D; i++)
			(*this)[i] = values[i] - r[i];

		return *this;
	}

	inline Vec<T, D>& operator*=(const T& r)
	{
		for (unsigned int i = 0; i < D; i++)
			(*this)[i] = values[i] * r;

		return *this;
	}

	inline Vec<T, D>& operator/=(const T& r)
	{
		for (unsigned int i = 0; i < D; i++)
			(*this)[i] = values[i] / r;

		return *this;
	}

	T& operator [] (unsigned int i) { return values[i]; }
	T operator [] (unsigned int i) const { return values[i]; }

	inline bool operator==(const Vec<T, D>& r) const
	{
		for (unsigned int i = 0; i < D; i++)
			if ((*this)[i] != r[i])
				return false;
		return true;
	}

	inline bool operator!=(const Vec<T, D>& r) const { return !operator==(r); }
protected:
private:
	T values[D];
};

template<typename T>
class Vec2 : public Vec<T, 2>
{
public:
	static const Vec2 zero;
	static const Vec2 one;

	Vec2() { }

	Vec2(const Vec<T, 2>& r)
	{
		(*this)[0] = r[0];
		(*this)[1] = r[1];
	}

	Vec2(T x, T y)
	{
		(*this)[0] = x;
		(*this)[1] = y;
	}

	T Cross(const Vec2<T>& r) const
	{
		return GetX() * r.GetY() - GetY() * r.GetX();
	}

	inline Vector2 raylib()
	{
		Vector2 vec;
		vec.x = (float)GetX();
		vec.y = (float)GetY();
		return vec;
	}

	std::string ToString() const;

	inline T GetX() const { return (*this)[0]; }
	inline T GetY() const { return (*this)[1]; }

	inline void SetX(const T& x) { (*this)[0] = x; }
	inline void SetY(const T& y) { (*this)[1] = y; }

	inline void Set(const T& x, const T& y) { SetX(x); SetY(y); }
protected:
private:
};

template<typename T>
class Vec3 : public Vec<T, 3>
{
public:
	Vec3() { }

	Vec3(const Vec<T, 3>& r)
	{
		(*this)[0] = r[0];
		(*this)[1] = r[1];
		(*this)[2] = r[2];
	}

	Vec3(T x, T y, T z)
	{
		(*this)[0] = x;
		(*this)[1] = y;
		(*this)[2] = z;
	}

	inline Vec3<T> Cross(const Vec3<T>& r) const
	{
		T x = (*this)[1] * r[2] - (*this)[2] * r[1];
		T y = (*this)[2] * r[0] - (*this)[0] * r[2];
		T z = (*this)[0] * r[1] - (*this)[1] * r[0];

		return Vec3<T>(x, y, z);
	}

	inline Vec3<T> Rotate(T angle, const Vec3<T>& axis) const
	{
		const T sinAngle = sin(-angle);
		const T cosAngle = cos(-angle);

		return this->Cross(axis * sinAngle) +        //Rotation on local X
			(*this * cosAngle) +                     //Rotation on local Z
			axis * this->Dot(axis * (1 - cosAngle)); //Rotation on local Y
	}

	inline Vec2<T> GetXY() const { return Vec2<T>(GetX(), GetY()); }
	inline Vec2<T> GetYZ() const { return Vec2<T>(GetY(), GetZ()); }
	inline Vec2<T> GetZX() const { return Vec2<T>(GetZ(), GetX()); }

	inline Vec2<T> GetYX() const { return Vec2<T>(GetY(), GetX()); }
	inline Vec2<T> GetZY() const { return Vec2<T>(GetZ(), GetY()); }
	inline Vec2<T> GetXZ() const { return Vec2<T>(GetX(), GetZ()); }

	inline T GetX() const { return (*this)[0]; }
	inline T GetY() const { return (*this)[1]; }
	inline T GetZ() const { return (*this)[2]; }

	inline void SetX(const T& x) { (*this)[0] = x; }
	inline void SetY(const T& y) { (*this)[1] = y; }
	inline void SetZ(const T& z) { (*this)[2] = z; }

	inline void Set(const T& x, const T& y, const T& z) { SetX(x); SetY(y); SetZ(z); }
protected:
private:
};

template<typename T>
class Vec4 : public Vec<T, 4>
{
public:
	Vec4() { }

	Vec4(const Vec<T, 4>& r)
	{
		(*this)[0] = r[0];
		(*this)[1] = r[1];
		(*this)[2] = r[2];
		(*this)[3] = r[3];
	}

	Vec4(T x, T y, T z, T w)
	{
		(*this)[0] = x;
		(*this)[1] = y;
		(*this)[2] = z;
		(*this)[3] = w;
	}

	inline T GetX() const { return (*this)[0]; }
	inline T GetY() const { return (*this)[1]; }
	inline T GetZ() const { return (*this)[2]; }
	inline T GetW() const { return (*this)[3]; }

	inline void SetX(const T& x) { (*this)[0] = x; }
	inline void SetY(const T& y) { (*this)[1] = y; }
	inline void SetZ(const T& z) { (*this)[2] = z; }
	inline void SetW(const T& w) { (*this)[3] = w; }

	inline void Set(const T& x, const T& y, const T& z, const T& w) { SetX(x); SetY(y); SetZ(z); SetW(w); }
protected:
private:
};

template<typename T, unsigned int D>
class Mat
{
public:
	inline Mat<T, D> InitIdentity()
	{
		for (unsigned int i = 0; i < D; i++)
		{
			for (unsigned int j = 0; j < D; j++)
			{
				if (i == j)
					m[i][j] = T(1);
				else
					m[i][j] = T(0);
			}
		}

		return *this;
	}

	inline Mat<T, D> InitScale(const Vec<T, D - 1>& r)
	{
		for (unsigned int i = 0; i < D; i++)
		{
			for (unsigned int j = 0; j < D; j++)
			{
				if (i == j && i != D - 1)
					m[i][j] = r[i];
				else
					m[i][j] = T(0);
			}
		}

		m[D - 1][D - 1] = T(1);

		return *this;
	}

	inline Mat<T, D> InitTranslation(const Vec<T, D - 1>& r)
	{
		for (unsigned int i = 0; i < D; i++)
		{
			for (unsigned int j = 0; j < D; j++)
			{
				if (i == D - 1 && j != D - 1)
					m[i][j] = r[j];
				else if (i == j)
					m[i][j] = T(1);
				else
					m[i][j] = T(0);
			}
		}

		m[D - 1][D - 1] = T(1);

		return *this;
	}

	inline Mat<T, D> Transpose() const
	{
		Mat<T, D> t;
		for (int j = 0; j < D; j++) {
			for (int i = 0; i < D; i++) {
				t[i][j] = m[j][i];
			}
		}
		return t;
	}

	// This function doesn't appear to work!
	inline Mat<T, D> Inverse() const
	{
		int i, j, k;
		Mat<T, D> s;
		Mat<T, D> t(*this);

		// Forward elimination
		for (i = 0; i < D - 1; i++) {
			int pivot = i;

			T pivotsize = t[i][i];

			if (pivotsize < 0)
				pivotsize = -pivotsize;

			for (j = i + 1; j < D; j++) {
				T tmp = t[j][i];

				if (tmp < 0)
					tmp = -tmp;

				if (tmp > pivotsize) {
					pivot = j;
					pivotsize = tmp;
				}
			}

			if (pivotsize == 0) {
				//if (singExc)
				//	throw ::Imath::SingMatrixExc ("Cannot invert singular matrix.");

				return Mat<T, D>();
			}

			if (pivot != i) {
				for (j = 0; j < D; j++) {
					T tmp;

					tmp = t[i][j];
					t[i][j] = t[pivot][j];
					t[pivot][j] = tmp;

					tmp = s[i][j];
					s[i][j] = s[pivot][j];
					s[pivot][j] = tmp;
				}
			}

			for (j = i + 1; j < D; j++) {
				T f = t[j][i] / t[i][i];

				for (k = 0; k < D; k++) {
					t[j][k] -= f * t[i][k];
					s[j][k] -= f * s[i][k];
				}
			}
		}

		// Backward substitution
		for (i = D - 1; i >= 0; --i) {
			T f;

			if ((f = t[i][i]) == 0) {
				//if (singExc)
				//	throw ::Imath::SingMatrixExc ("Cannot invert singular matrix.");

				return Mat<T, D>();
			}

			for (j = 0; j < D; j++) {
				t[i][j] /= f;
				s[i][j] /= f;
			}

			for (j = 0; j < i; j++) {
				f = t[j][i];

				for (k = 0; k < D; k++) {
					t[j][k] -= f * t[i][k];
					s[j][k] -= f * s[i][k];
				}
			}
		}

		return s;
	}

	inline Mat<T, D> operator*(const Mat<T, D>& r) const
	{
		Mat<T, D> ret;
		for (unsigned int i = 0; i < D; i++)
		{
			for (unsigned int j = 0; j < D; j++)
			{
				ret.m[i][j] = T(0);
				for (unsigned int k = 0; k < D; k++)
					ret.m[i][j] += m[k][j] * r.m[i][k];
			}
		}
		return ret;
	}

	inline Vec<T, D> Transform(const Vec<T, D>& r) const
	{
		Vec<T, D> ret;

		for (unsigned int i = 0; i < D; i++)
		{
			ret[i] = 0;
			for (unsigned int j = 0; j < D; j++)
				ret[i] += m[j][i] * r[j];
		}

		return ret;
	}

	inline Vec<T, D - 1> Transform(const Vec<T, D - 1>& r) const
	{
		Vec<T, D> r2;

		for (int i = 0; i < D - 1; i++)
			r2[i] = r[i];

		r2[D - 1] = T(1);

		Vec<T, D> ret2 = Transform(r2);
		Vec<T, D - 1> ret;

		for (int i = 0; i < D - 1; i++)
			ret[i] = ret2[i];

		return ret;
	}

	inline void Set(unsigned int x, unsigned int y, T val) { m[x][y] = val; }

	inline const T* operator[](int index) const { return m[index]; }
	inline T* operator[](int index) { return m[index]; }
protected:
private:
	T m[D][D];
};

template<typename T>
class Mat4 : public Mat<T, 4>
{
public:
	Mat4() { }

	template<unsigned int D>
	Mat4(const Mat<T, D>& r)
	{
		if (D < 4)
		{
			this->InitIdentity();

			for (unsigned int i = 0; i < D; i++)
				for (unsigned int j = 0; j < D; j++)
					(*this)[i][j] = r[i][j];
		}
		else
		{
			for (unsigned int i = 0; i < 4; i++)
				for (unsigned int j = 0; j < 4; j++)
					(*this)[i][j] = r[i][j];
		}
	}

	inline Mat4<T> InitRotationEuler(T rotateX, T rotateY, T rotateZ)
	{
		Mat4<T> rx, ry, rz;

		const T x = rotateX;
		const T y = rotateY;
		const T z = rotateZ;

		rx[0][0] = T(1);   rx[1][0] = T(0);  rx[2][0] = T(0); rx[3][0] = T(0);
		rx[0][1] = T(0);   rx[1][1] = cos(x);  rx[2][1] = -sin(x); rx[3][1] = T(0);
		rx[0][2] = T(0);   rx[1][2] = sin(x);  rx[2][2] = cos(x); rx[3][2] = T(0);
		rx[0][3] = T(0);   rx[1][3] = T(0);  rx[2][3] = T(0); rx[3][3] = T(1);

		ry[0][0] = cos(y); ry[1][0] = T(0);    ry[2][0] = -sin(y); ry[3][0] = T(0);
		ry[0][1] = T(0); ry[1][1] = T(1);    ry[2][1] = T(0); ry[3][1] = T(0);
		ry[0][2] = sin(y); ry[1][2] = T(0);    ry[2][2] = cos(y); ry[3][2] = T(0);
		ry[0][3] = T(0); ry[1][3] = T(0);    ry[2][3] = T(0); ry[3][3] = T(1);

		rz[0][0] = cos(z); rz[1][0] = -sin(z); rz[2][0] = T(0);    rz[3][0] = T(0);
		rz[0][1] = sin(z); rz[1][1] = cos(z); rz[2][1] = T(0);    rz[3][1] = T(0);
		rz[0][2] = T(0); rz[1][2] = T(0); rz[2][2] = T(1);    rz[3][2] = T(0);
		rz[0][3] = T(0); rz[1][3] = T(0); rz[2][3] = T(0);    rz[3][3] = T(1);

		*this = rz * ry * rx;

		return *this;
	}

	inline Mat4<T> InitRotationFromVectors(const Vec3<T>& n, const Vec3<T>& v, const Vec3<T>& u)
	{
		(*this)[0][0] = u.GetX();   (*this)[1][0] = u.GetY();   (*this)[2][0] = u.GetZ();   (*this)[3][0] = T(0);
		(*this)[0][1] = v.GetX();   (*this)[1][1] = v.GetY();   (*this)[2][1] = v.GetZ();   (*this)[3][1] = T(0);
		(*this)[0][2] = n.GetX();   (*this)[1][2] = n.GetY();   (*this)[2][2] = n.GetZ();   (*this)[3][2] = T(0);
		(*this)[0][3] = T(0);       (*this)[1][3] = T(0);       (*this)[2][3] = T(0);       (*this)[3][3] = T(1);

		return *this;
	}

	inline Mat4<T> InitRotationFromDirection(const Vec3<T>& forward, const Vec3<T>& up)
	{
		Vec3<T> n = forward.Normalized();
		Vec3<T> u = Vec3<T>(up.Normalized()).Cross(n);
		Vec3<T> v = n.Cross(u);

		return InitRotationFromVectors(n, v, u);
	}

	inline Mat4<T> InitPerspective(T fov, T aspectRatio, T zNear, T zFar)
	{
		const T zRange = zNear - zFar;
		const T tanHalfFOV = tanf(fov / T(2));

		(*this)[0][0] = T(1) / (tanHalfFOV * aspectRatio); (*this)[1][0] = T(0);   (*this)[2][0] = T(0);            (*this)[3][0] = T(0);
		(*this)[0][1] = T(0);                   (*this)[1][1] = T(1) / tanHalfFOV; (*this)[2][1] = T(0);            (*this)[3][1] = T(0);
		(*this)[0][2] = T(0);                   (*this)[1][2] = T(0);            (*this)[2][2] = (-zNear - zFar) / zRange; (*this)[3][2] = T(2) * zFar * zNear / zRange;
		(*this)[0][3] = T(0);                   (*this)[1][3] = T(0);            (*this)[2][3] = T(1);            (*this)[3][3] = T(0);

		return *this;
	}

	inline Mat4<T> InitOrthographic(T left, T right, T bottom, T top, T near, T far)
	{
		const T width = (right - left);
		const T height = (top - bottom);
		const T depth = (far - near);

		(*this)[0][0] = T(2) / width; (*this)[1][0] = T(0);        (*this)[2][0] = T(0);        (*this)[3][0] = -(right + left) / width;
		(*this)[0][1] = T(0);       (*this)[1][1] = T(2) / height; (*this)[2][1] = T(0);        (*this)[3][1] = -(top + bottom) / height;
		(*this)[0][2] = T(0);       (*this)[1][2] = T(0);        (*this)[2][2] = T(-2) / depth; (*this)[3][2] = -(far + near) / depth;
		(*this)[0][3] = T(0);       (*this)[1][3] = T(0);        (*this)[2][3] = T(0);        (*this)[3][3] = T(1);

		return *this;
	}
protected:
private:
};

template<typename T>
class Mat3 : public Mat<T, 3>
{
public:
	Mat3() { }

	template<unsigned int D>
	Mat3(const Mat<T, D>& r)
	{
		if (D < 3)
		{
			this->InitIdentity();

			for (unsigned int i = 0; i < D; i++)
				for (unsigned int j = 0; j < D; j++)
					(*this)[i][j] = r[i][j];
		}
		else
		{
			for (unsigned int i = 0; i < 3; i++)
				for (unsigned int j = 0; j < 3; j++)
					(*this)[i][j] = r[i][j];
		}
	}
};

class Vec3f : public Vec3<float>
{
public:
	static const Vec3f zero;
	static const Vec3f one;

	Vec3f(float x = 0.0f, float y = 0.0f, float z = 0.0f)
	{
		(*this)[0] = x;
		(*this)[1] = y;
		(*this)[2] = z;
	}

	Vec3f(const Vec3<float>& r)
	{
		(*this)[0] = r[0];
		(*this)[1] = r[1];
		(*this)[2] = r[2];
	}

	inline float Length() const { return sqrtf(GetX() * GetX() + GetY() * GetY() + GetZ() * GetZ()); }
	inline float Dot(const Vec3f& v) const { return GetX() * v.GetX() + GetY() * v.GetY() + GetZ() * v.GetZ(); }

	inline Vec3f Cross(const Vec3f& v) const
	{
		const float _x = GetY() * v.GetZ() - GetZ() * v.GetY();
		const float _y = GetZ() * v.GetX() - GetX() * v.GetZ();
		const float _z = GetX() * v.GetY() - GetY() * v.GetX();

		return Vec3f(_x, _y, _z);
	}

	inline Vec3f Normalized() const
	{
		const float length = Length();

		return Vec3f(GetX() / length, GetY() / length, GetZ() / length);
	}

	inline Vec3f operator+(const Vec3f& r) const { return Vec3f(GetX() + r.GetX(), GetY() + r.GetY(), GetZ() + r.GetZ()); }
	inline Vec3f operator-(const Vec3f& r) const { return Vec3f(GetX() - r.GetX(), GetY() - r.GetY(), GetZ() - r.GetZ()); }
	inline Vec3f operator*(float f) const { return Vec3f(GetX() * f, GetY() * f, GetZ() * f); }
	inline Vec3f operator/(float f) const { return Vec3f(GetX() / f, GetY() / f, GetZ() / f); }

	inline bool operator==(const Vec3f& r) const { return GetX() == r.GetX() && GetY() == r.GetY() && GetZ() == r.GetZ(); }
	inline bool operator!=(const Vec3f& r) const { return !operator==(r); }

	inline Vec3f& operator+=(const Vec3f& r)
	{
		(*this)[0] += r.GetX();
		(*this)[1] += r.GetY();
		(*this)[2] += r.GetZ();

		return *this;
	}

	inline Vec3f& operator-=(const Vec3f& r)
	{
		(*this)[0] -= r.GetX();
		(*this)[1] -= r.GetY();
		(*this)[2] -= r.GetZ();

		return *this;
	}

	inline Vec3f& operator*=(float f)
	{
		(*this)[0] *= f;
		(*this)[1] *= f;
		(*this)[2] *= f;

		return *this;
	}

	inline Vec3f& operator/=(float f)
	{
		(*this)[0] /= f;
		(*this)[1] /= f;
		(*this)[2] /= f;

		return *this;
	}

	inline Vector3 raylib()
	{
		Vector3 vec;
		vec.x = GetX();
		vec.y = GetY();
		vec.z = GetZ();
		return vec;
	}

	std::string ToString() const;

	inline float GetX() const { return (*this)[0]; }
	inline float GetY() const { return (*this)[1]; }
	inline float GetZ() const { return (*this)[2]; }

	inline void SetX(float x) { (*this)[0] = x; }
	inline void SetY(float y) { (*this)[1] = y; }
	inline void SetZ(float z) { (*this)[2] = z; }

	inline void Set(float x, float y, float z) { (*this)[0] = x; (*this)[1] = y; (*this)[2] = z; }
};

typedef Vec2<int> Vec2i;
typedef Vec3<int> Vec3i;
typedef Vec4<int> Vec4i;

typedef Vec2<float> Vec2f;
typedef Vec4<float> Vec4f;

typedef Vec2<double> Vec2d;
typedef Vec3<double> Vec3d;
typedef Vec4<double> Vec4d;

typedef Mat<int, 2> Mat2i;
typedef Mat3<int> Mat3i;
typedef Mat4<int> Mat4i;

typedef Mat<float, 2> Mat2f;
typedef Mat3<float> Mat3f;
typedef Mat4<float> Mat4f;

typedef Mat<double, 2> Mat2d;
typedef Mat3<double> Mat3d;
typedef Mat4<double> Mat4d;