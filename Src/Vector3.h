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
	Vector3 operator*(float s) const { return { x * s,   y * s,   z * s }; }
	Vector3 operator/(float s) const { 

		// 0으로 나누기 방지 로직과 연산 비용 최적화
		float invS = 1.0f / s;
		return { x * invS,   y * invS,   z * invS };
	}

	Vector3& operator+=(const Vector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
	Vector3& operator-=(const Vector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
	Vector3& operator*=(float s) { x *= s;y *= s;z *= s;return *this; }

	float Length() const { return std::sqrt(LengthSquared()); }
	float LengthSquared() const { return (x * x + y * y + z * z); }

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

// 왼쪽 항이 클래스(구조체)가 아닌 기본 자료형(float)일 경우, 멤버 함수가 아닌 전역 함수로 정의
inline Vector3 operator*(float s, const Vector3& v)
{
	return v * s;
}

inline Vector3 operator/(float s, const Vector3& v)
{
	return v / s;
}
