#pragma once
#include<memory>
#include "IUpdate.h"
#include"IRender.h"
#include <chrono>

class RenderManager;
class IGame;
class SceneManager;

class GameEngine
{
private:
	GameEngine();
	static std::unique_ptr<GameEngine> m_instance;

public:
	~GameEngine();

	static GameEngine* Instance()
	{
		if (m_instance == nullptr)
		{
			m_instance.reset(new GameEngine());
		}

		return m_instance.get();
	}

	// init
	bool Init(std::unique_ptr<IGame> scene);

	// running
	void Run();

	// clean
	void Release();

	void Tick();

	void CapFrame(std::chrono::steady_clock::time_point frameStart);

	// running
	//bool IsRunning() { return m_isRunning; };

	RenderManager* GetRenderManager() const { return m_renderManager.get(); }

private:
	float m_deltaTime;
	std::chrono::steady_clock::time_point m_prevTime;

	bool m_isRunning;
	std::unique_ptr<RenderManager> m_renderManager;
	std::unique_ptr<IGame> m_game;



};