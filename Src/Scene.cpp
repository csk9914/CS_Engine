#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include "IRenderer.h"
#include "MeshRenderer.h"


#include "MeshData.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "MeshFilter.h"

Scene::~Scene()
{
}

void Scene::Update(float deltaTime)
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

void Scene::OnEnter()
{

}

void Scene::OnExit()
{

}

GameObject* Scene::CreateGameObject(const std::string& name )
{
	auto newObj = std::make_unique<GameObject>(name);
	GameObject* ptr = newObj.get();

	m_gameObjects.push_back(std::move(newObj));

	return ptr;
}
