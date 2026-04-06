#include "InputHandler.h"


std::unique_ptr< InputHandler> InputHandler::m_instance = nullptr;

InputHandler::InputHandler()
{
}


void InputHandler::Update()
{
	// 마우스 위치 & 델타
	m_prevPos = m_mousePos;
	GetCursorPos(&m_mousePos);

	if (m_hWnd) ScreenToClient(m_hWnd, &m_mousePos);

	m_mouseDelta.x = m_mousePos.x - m_prevPos.x;
	m_mouseDelta.y = m_mousePos.y - m_prevPos.y;
}

void InputHandler::EndFrame()
{
	// 휠은 WM_MOUSEWHEEL에서 SetWheelDelta로 누적되므로
// 매 프레임 끝에 0으로 초기화
	m_mouseWheel = 0.0f;
}

bool InputHandler::IsKeyDown(int key)
{
	return (GetAsyncKeyState(key) & 0x8000) != 0;

}

bool InputHandler::IsKeyOneDown(int key)
{
	if (IsKeyDown(key))
	{
		if (m_checkOneDown[key] == false)
		{
			m_checkOneDown[key] = true;
			return true;
		}
		return false;
	}
	else
	{
		m_checkOneDown[key] = false;
		return false;
	}
}
