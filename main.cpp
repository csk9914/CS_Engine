#include"GameEngine.h"
#include "TEXT_RPG.h"
#include "SHOOTER.h"

#include<iostream>
#include <memory>

const int WIDTH = 100;
const int HEIGHT = 25;

int main()
{
	if (GameEngine::Instance()->Init(std::unique_ptr<IGame> (new SHOOTER()), WIDTH, HEIGHT))
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