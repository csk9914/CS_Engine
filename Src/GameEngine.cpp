#include "Defines.h"
#include "GameEngine.h"
#include "CSWindow.h"

#include "GameObject.h"
#include "GameScene.h"
#include "IGame.h"
#include "InputHandler.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "DX11Renderer.h"
#include "EditorUI.h"
#include "SHOOTER.h"

#include<conio.h>

#include <chrono>
#include <iostream>
#include<memory>
#include <thread>


using namespace std;

std::unique_ptr<GameEngine> GameEngine::m_instance = nullptr;

GameEngine::GameEngine() {}

GameEngine::~GameEngine() {}

bool GameEngine::Init(std::unique_ptr<IGame> game)
{
	if (!game) return false;
	m_game = std::move(game);

	// 윈도우 생성
	m_window = std::make_unique<CSWindow>();

	std::wstring title(m_game->GetAppConfig().title.begin(), m_game->GetAppConfig().title.end());

	if (!m_window->Init(title, m_game->GetWidth(), m_game->GetHeight(), GetModuleHandle(nullptr))) return false;


	// 윈도우 핸들 전달
	InputHandler::Instance()->SetHWND(m_window->GetHWND());

	//  DX11 RenderManager 생성 
	m_renderManager = std::make_unique<RenderManager>(m_window->GetHWND(), m_window->GetWidth(), m_window->GetHeight());
	if (!m_renderManager->GetRenderer()) return false;


	m_isRunning = true;
	return true;
}

void GameEngine::Run()
{
	// 초기화
	m_game->Start();
	m_prevTime = std::chrono::steady_clock::now();

	while (m_isRunning)
	{
		// 프레임 시작 시간 기록
		auto frameStart = std::chrono::steady_clock::now();

		// 메시지 펌프 (WM_QUIT → 루프 종료)
		if (!m_window->PumpMessages()) { m_isRunning = false; break; }

		// 게임 시간 관리
		Tick();

		// 입력 처리
		InputHandler::Instance()->Update();

		// 게임 업데이트
		m_game->Update(m_deltaTime);

		// 렌더링
		m_renderManager->RenderAll(m_deltaTime);

		// end 초기화
		InputHandler::Instance()->EndFrame();

		// 프레임 지연
		CapFrame(frameStart);
	}

	m_game->End();
}

void GameEngine::Release()
{
	m_game.reset();
	m_renderManager->Release();
	m_renderManager.reset();
	m_window.reset();
}

GameScene* GameEngine::GetCurrentScene()
{
	return m_game ? m_game->GetCurrentScene() : nullptr;
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
	const std::chrono::duration<double> TARGET_FRAME_TIME(1.0 / m_game->GetAppConfig().fps);

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

