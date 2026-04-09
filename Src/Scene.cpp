#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include "IRenderer.h"
#include "MeshRenderer.h"


#include "MeshData.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "MeshFilter.h"
#include "Rigidbody.h"
#include "GameApp.h"

#include "GameEngine.h"

#include "CollisionManager.h"
Scene::~Scene() {}

void Scene::Update(float deltaTime)
{


	ProcessAwake();

	ProcessStart();

	ProcessFixedUpdate(deltaTime);

	ProcessUpdate(deltaTime);

	ProcessDestroy();
}

void Scene::ProcessAwake()
{
	if (m_awakeQueue.empty()) return;

	for (auto obj : m_awakeQueue.prepare())
	{
		obj->Awake();

		m_startQueue.push(obj);
	}
}

void Scene::ProcessStart()
{
	if (m_startQueue.empty()) return;

	for (auto obj : m_startQueue.prepare())
	{
		obj->Start();
	}
}

void Scene::ProcessFixedUpdate(float deltaTime)
{
	CollisionManager::Instance()->Update();

	for (auto obj : m_rigids)
	{
		obj->FixedUpdate(deltaTime);
	}
}

void Scene::ProcessUpdate(float deltaTime)
{
	for (auto& obj : m_gameObjects)
	{
		obj->Update(deltaTime);
	}
}

void Scene::ProcessDestroy()
{
	// 죽은 객체만 골라서 삭제 (백터에서 제거)
	m_gameObjects.erase(std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
		[](const std::unique_ptr<GameObject>& obj)
		{
			if (obj->IsDestroy()) { obj->OnDestroy(); return true; }
			return false;
		}),
		m_gameObjects.end());
}


void Scene::OnEnter() {}

void Scene::OnExit()
{
	m_gameObjects.clear();
	m_awakeQueue.clear();
	m_startQueue.clear();
}

GameObject* Scene::CreateGameObject(const std::string& name)
{
	auto newObj = std::make_unique<GameObject>(this, name);
	GameObject* ptr = newObj.get();

	m_gameObjects.push_back(std::move(newObj));
	m_awakeQueue.push(ptr);

	return ptr;
}

void Scene::RegisterForRigidbody(Rigidbody* rigid)
{
	if (std::find(m_rigids.begin(), m_rigids.end(), rigid) == m_rigids.end())
		m_rigids.emplace_back(rigid);
}

void Scene::UnRegisterForRigidbody(Rigidbody* rigid)
{
	m_rigids.erase(std::remove(m_rigids.begin(), m_rigids.end(), rigid), m_rigids.end());
}

bool Scene::IsPlaying() const
{
	return GameEngine::Instance()->GetGameApp()->IsPlay();
}
