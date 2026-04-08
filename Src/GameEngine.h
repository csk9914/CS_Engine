#pragma once
#include<memory>
#include "IUpdate.h"
#include"IRender.h"
#include <chrono>

class Scene;
class CSWindow;
class RenderManager;
class EditorUI;
class Camera;
class GameApp;


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
	bool Init(std::unique_ptr<GameApp> scene);

	// running
	void Run();

	// clean
	void Release();


	RenderManager* GetRenderManager() const { return m_renderManager.get(); }

	void SetRenderCamera(Camera* cam) { m_renderCamera = cam; }
	Camera* GetRenderCamera() { return m_renderCamera; }

	// MeshRenderer에서 카메라 행렬을 가져가기 위한 접근자
	Camera* GetEditorCamera() const { return m_editorCamera; }
	void SetEditorCamera(Camera* cam) { m_editorCamera = cam; }

	// 실제 게임용 카메라는 나중에 m_gameMainCamera 등으로 따로 만듭니다.
	void SetGameCamera(Camera* cam) { m_gameCamera = cam; }
	Camera* GetGameCamera() const { return m_gameCamera; }

	GameApp* GetGameApp() const { return m_gameApp.get(); }

	Scene* GetCurrentScene();

private:
	void Tick();
	void CapFrame(std::chrono::steady_clock::time_point frameStart);


private:
	bool  m_isRunning = false;
	float m_deltaTime = 0.0f;
	std::chrono::steady_clock::time_point m_prevTime;


	std::unique_ptr<CSWindow> m_window;
	std::unique_ptr<RenderManager> m_renderManager;
	std::unique_ptr<GameApp> m_gameApp;


	Camera* m_renderCamera = nullptr; // 현재 렌더링 중인 카메라
	Camera* m_editorCamera = nullptr; // 소유권은 GameObject에 있음
	Camera* m_gameCamera = nullptr;
};