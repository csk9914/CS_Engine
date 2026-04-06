#pragma once
#include <Windows.h>
#include <string>

class CSWindow
{
public:
	CSWindow() = default;
	~CSWindow();

	bool Init(const std::wstring& title, int width, int height, HINSTANCE hInstance);

	// 윈도우 메시지를 처리
	bool PumpMessages();

public:
	HWND GetHWND() const { return m_hWnd; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	HWND m_hWnd = nullptr;
	HINSTANCE m_hInst = nullptr;
	int m_width = 0;
	int m_height = 0;
};
