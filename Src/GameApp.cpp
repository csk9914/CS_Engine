#include "GameApp.h"



void GameApp::Update(float deltaTime)
{
	if (m_pendingScene)
	{
		m_sceneMachine.ChangeState(std::move(m_pendingScene));
	}

	if (m_isPlay)
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

void GameApp::SetIsPlay(bool play)
{
	if (m_isPlay == play) return; // 중복 호출 방지
	m_isPlay = play;

	Scene* scene = GetCurrentScene();
	if (!scene) return;

	if (m_isPlay)
		scene->OnPlayStart(); // 에디터에서 추가된 컴포넌트 OnEnable 재호출
	else
		scene->OnPlayStop();  // 물리 상태 초기화

}
