#include "GameApp.h"



void GameApp::Update(float deltaTime)
{
	if (m_pendingScene)
	{
		m_sceneMachine.ChangeState(std::move(m_pendingScene));
	}

	m_sceneMachine.Update(deltaTime);
}

void GameApp::End()
{
	m_sceneMachine.ChangeState(nullptr); 
}

void GameApp::SetFirstScene(std::unique_ptr<Scene> scene)
{
	m_pendingScene = std::move(scene);
}
