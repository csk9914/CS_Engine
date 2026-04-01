#include"GameEngine.h"
#include "TEXT_RPG.h"
#include "SHOOTER.h"

#include<iostream>
#include <memory>





int main()
{
	if (GameEngine::Instance()->Init(std::unique_ptr<IGame> (new SHOOTER())))
	{
		GameEngine::Instance()->Run();
	}
	else
	{
		// 초기화 실패
		std::cout << "초기화 실패";
	}

	GameEngine::Instance()->Release();
}