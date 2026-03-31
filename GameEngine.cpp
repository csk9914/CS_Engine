#include "Defines.h"
#include "GameEngine.h"
#include "GameObject.h"
#include "GameScene.h"
#include "IGame.h"
#include "InputHandler.h"
#include "Renderer.h"
#include "SceneManager.h"

#include<conio.h>

#include <chrono>
#include <iostream>
#include<memory>
#include <thread>





using namespace std;

std::unique_ptr<GameEngine> GameEngine::m_instance = nullptr;

GameEngine::GameEngine() : m_deltaTime(0.0f), m_isRunning(false) {}

GameEngine::~GameEngine() {}

bool GameEngine::Init(std::unique_ptr<IGame> scene, int width, int height)
{
	// 렌더러 생성
	m_renderer = Renderer::CreateRenderer(width, height, false);
	if (m_renderer == nullptr) return false;

	// 게임 씬 생성( 게임 구현부)
	m_gameScene = std::move(scene);
	if (m_gameScene == nullptr) return false;

	m_isRunning = true;

	return true;
}

void GameEngine::Run()
{
	// 초기화
	m_gameScene->Start();

	m_prevTime = std::chrono::steady_clock::now();
	while (m_isRunning)
	{
		// 프레임 시작 시간 기록
		auto frameStart = std::chrono::steady_clock::now();

		Tick();
		InputHandler::Instance()->Update();
		m_gameScene->Update(m_deltaTime);
		m_gameScene->Render(*m_renderer);
		m_renderer->Flip();


		// 프레임 지연
		CapFrame(frameStart);
	}

	m_gameScene->End();
}

void GameEngine::Release()
{

}

void GameEngine::Tick()
{
	auto nowTime = std::chrono::steady_clock::now();
	float frameTime = std::chrono::duration<float>(nowTime - m_prevTime).count();
	m_deltaTime = frameTime > MAX_DELTA_TIME ? MAX_DELTA_TIME : frameTime;
	m_prevTime = nowTime;
}

void GameEngine::CapFrame(std::chrono::steady_clock::time_point frameStart)
{
	// 목표 프레임 시간
	const std::chrono::duration<double> TARGET_FRAME_TIME(1.0 / FPS);

	// 프레임 제한(Capping)
	auto frameEnd = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = frameEnd - frameStart;

	// 3. 시간이 남았다면 대기
	if (elapsed < TARGET_FRAME_TIME)
	{
		// OS에게 제어권을 넘겨 CPU 점유율을 낮춤
		std::this_thread::sleep_for(TARGET_FRAME_TIME - elapsed);
	}
}

