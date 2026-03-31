#pragma once

class LoaderParams
{
public:
	LoaderParams(float x, float y, float width, float height)
		: m_x(x), m_y(y), m_width(width), m_height(height) { }

	float GetX() const { return m_x; }
	float GetY() const { return m_y; }
	float GetWidth() const{ return m_width; }
	float GetHeight() const { return m_height; }

private:
	float m_x, m_y;
	float m_width, m_height;
};