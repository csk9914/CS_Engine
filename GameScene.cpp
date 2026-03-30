#include "GameScene.h"
#include "GameObject.h"
#include "IRenderer.h"

void GameScene::Update(float deltaTime)
{
	// 모든 객체 업데이트
	for (auto& obj : m_gameObjects)
	{
		obj->Update(deltaTime);
	}

	// 죽은 객체만 골라서 삭제 (백터에서 제거)
	m_gameObjects.erase(std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
		[](const std::unique_ptr<GameObject>& obj)
		{
			if (obj->IsDestroy())
			{
				// 파괴 시 콜백 실행
				obj->OnDestroy();
				return true;
			}
			return false;
		}));

}

void GameScene::Render(IRenderer& renderer) const
{
	for (auto& obj : m_gameObjects)
	{
		obj->Render(renderer);
	}
}

void GameScene::OnEnter()
{

}

void GameScene::OnExit()
{

}
