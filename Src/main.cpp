#include"GameEngine.h"
#include "SHOOTER.h"

#include <Windows.h>
#include<iostream>
#include <memory>


int WINAPI WinMain(
	_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPSTR     lpCmdLine,
	_In_     int       nCmdShow)
{
	if (GameEngine::Instance()->Init(std::make_unique<SHOOTER>()))
	{
		GameEngine::Instance()->Run();
	}

	GameEngine::Instance()->Release();
	return 0;
}


