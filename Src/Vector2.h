#pragma once
#include <cmath>

struct Vector2
{
	float x, y;

	Vector2() : x(0), y(0) {}
	Vector2(float x, float y) : x(x), y(y) {}



	Vector2 operator+(const Vector2& o) const { return { x + o.x, y + o.y }; }
	Vector2 operator-(const Vector2& o) const { return { x - o.x, y - o.y }; }
	Vector2 operator*(float s)          const { return { x * s,   y * s }; }
	Vector2& operator+=(const Vector2& o) { x += o.x; y += o.y; return *this; }
	Vector2& operator-=(const Vector2& o) { x -= o.x; y -= o.y;return *this; }
	Vector2& operator*=(const Vector2& o) { x *= o.x; y *= o.y;return *this; }

	float Length() const { return std::sqrt(x * x + y * y); }
	Vector2 Normalized() const
	{
		float len = Length();
		if (len < 1e-6f) return {};
		return { x / len, y / len };
	}

};