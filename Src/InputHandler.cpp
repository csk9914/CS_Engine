#include "InputHandler.h"


std::unique_ptr< InputHandler> InputHandler::m_instance = nullptr;

InputHandler::InputHandler()
{
}


void InputHandler::Update()
{
	// 마우스
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
