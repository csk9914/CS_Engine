#pragma once
#include <string>

struct AppConfig
{
	std::string title;
	int width;
	int height;
	float fps;
	bool isCursorVisible;

	AppConfig(const std::string& title = "CS Engine", int width = 100, int height = 25, float fps = 60.0f, bool isCursorVisible = false)
		: title(title), width(width), height(height), fps(fps), isCursorVisible(isCursorVisible)
	{
	}		
};