#include"GameEngine.h"
#include "SHOOTER.h"

#include <Windows.h>
#include<iostream>
#include <memory>


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// 콘솔 창을 생성하고 표준 입출력을 연결
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	std::cout << "Engine Console Initialized!" << std::endl;

	if (GameEngine::Instance()->Init(std::make_unique<SHOOTER>()))
	{
		GameEngine::Instance()->Run();
	}

	GameEngine::Instance()->Release();

	return 0;
}


