#pragma once

#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <memory>

#include"Vector2.h"
#include "IUpdate.h"

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

	void Update() ;
	bool IsKeyOneDown(int key);
	bool IsKeyDown(int key);

private:
	std::unordered_map<int, bool> m_checkOneDown;

};