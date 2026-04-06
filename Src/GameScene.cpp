#include "GameScene.h"
#include "GameObject.h"
#include "Component.h"
#include "IRenderer.h"
#include "MeshRenderer.h"

GameScene::~GameScene()
{
}

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
			if (obj->IsDestroy()) { obj->OnDestroy(); return true; }
			return false;
		}),
		m_gameObjects.end());



}

//void GameScene::Render() const
//{
//	for (auto& obj : m_gameObjects)
//	{
//		if (!obj->GetActive()) continue;
//
//		MeshRenderer* meshRenderer = obj->GetComponent<MeshRenderer>();
//
//		if (meshRenderer)
//			meshRenderer->Render();
//	}
//}


void GameScene::OnEnter()
{

}

void GameScene::OnExit()
{

}

GameObject* GameScene::AddGameObject(const std::string& name )
{
	auto obj = std::make_unique<GameObject>(name);
	GameObject* ptr = obj.get();
	m_gameObjects.push_back(std::move(obj));
	return ptr;
}
