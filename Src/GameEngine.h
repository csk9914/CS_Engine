#pragma once
#include<memory>
#include "IUpdate.h"
#include"IRender.h"
#include <chrono>

class GameScene;
class CSWindow;
class RenderManager;
class EditorUI;
class Camera;
class IGame;


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


	RenderManager* GetRenderManager() const { return m_renderManager.get(); }

	// MeshRenderer에서 카메라 행렬을 가져가기 위한 접근자
	Camera* GetMainCamera() const { return m_mainCamera; }
	void SetMainCamera(Camera* cam) { m_mainCamera = cam; }

	GameScene* GetCurrentScene();

private:
	void Tick();
	void CapFrame(std::chrono::steady_clock::time_point frameStart);


private:
	bool  m_isRunning = false;
	float m_deltaTime = 0.0f;
	std::chrono::steady_clock::time_point m_prevTime;


	std::unique_ptr<CSWindow> m_window;
	std::unique_ptr<RenderManager> m_renderManager;
	//std::unique_ptr<EditorUI>      m_editorUI;
	std::unique_ptr<IGame> m_game;


	Camera* m_mainCamera = nullptr; // 소유권은 GameObject에 있음
};