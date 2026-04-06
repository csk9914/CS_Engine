#pragma once

#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <memory>

#include"Vector3.h"

class InputHandler  
{
private:
	InputHandler();
	static std::unique_ptr<InputHandler> m_instance;

public:
	~InputHandler() {}

	static InputHandler* Instance()
	{
		if (m_instance == nullptr)
		{
			m_instance.reset(new InputHandler());
		}

		return m_instance.get();
	}


public:
	void Update() ;

	// 키보드
	bool IsKeyOneDown(int key);
	bool IsKeyDown(int key);

	// 마우스
	POINT GetMousePos()   const { return m_mousePos; }
	POINT GetMouseDelta() const { return m_mouseDelta; }
	float GetMouseWheel() const { return m_mouseWheel; }

	// GameEngine에서 초기화 시 윈도우 핸들 전달
	void SetHWND(HWND hWnd) { m_hWnd = hWnd; }

	// WndProc에서 호출 (WM_MOUSEWHEEL)
	void SetWheelDelta(float delta) { m_mouseWheel = delta; }

private:
	std::unordered_map<int, bool> m_checkOneDown;

	HWND  m_hWnd = nullptr;
	POINT m_mousePos = {};
	POINT m_prevPos = {};
	POINT m_mouseDelta = {};
	float m_mouseWheel = 0.0f;
};