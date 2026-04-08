#pragma once
#include <cmath>
#include <algorithm>

struct Vector3
{
	float x, y, z;

	Vector3() : x(0), y(0), z(0) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	Vector3 operator+(const Vector3& o) const { return { x + o.x, y + o.y, z + o.z }; }
	Vector3 operator-(const Vector3& o) const { return { x - o.x, y - o.y, z - o.z }; }
	Vector3 operator*(float s)          const { return { x * s,   y * s,   z * s }; }
	Vector3& operator+=(const Vector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
	Vector3& operator-=(const Vector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
	Vector3& operator*=(float s) { x *= s;y *= s;z *= s;return *this; }

	float Length() const { return std::sqrt(x * x + y * y + z * z); }
	Vector3 Normalized() const
	{
		float len = Length();
		if (len < 1e-6f) return {};
		return { x / len, y / len, z / len };
	}

	float MaxElement() const
	{
		return (std::max)({ x, y, z });
	}

	static float DistanceSquared(const Vector3& a, const Vector3& b)
	{
		Vector3 dis = a - b;
		return dis.x * dis.x + dis.y * dis.y + dis.z * dis.z;
	}

	static float Dot(const Vector3& a, const Vector3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}


};