#include "GameObject.h"
#include "SceneManager.h"
#include "PrimitiveType.h"
#include "ResourceManager.h"

#include "MeshData.h"
#include "Mesh.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"

GameObject::GameObject(const std::string& name) : m_name(name), m_active(true), m_isDestroyed(false)
{
	m_transform = AddComponent<Transform>();
}

// 객체가 파괴될 때 자원 정리
void GameObject::OnDestroy()
{
	for (auto& comp : m_components)
	{
		comp->OnDestroy();
	}
	m_components.clear();
}

void GameObject::Update(float deltaTime)
{
	if (!m_active) return;

	for (auto& comp : m_components)
	{
		// 아직 시작되지 않은 컴포넌트는 Start() 호출
		if (!comp->m_started)
		{
			comp->m_started = true;
			comp->Start();
		}

		// 활성화된 컴포넌트만 Update() 호출
		if (comp->m_enabled)
		{
			comp->Update(deltaTime);
		}
	}
}

GameObject* GameObject::CreateGameObject(const std::string& name)
{
	return SceneManager::Instantiate(name);
}

GameObject* GameObject::CreatePrimitive(PrimitiveType type)
{
	GameObject* obj = SceneManager::Instantiate(ResourceManager::Instance()->GetPrimitiveNames(type));
	
	obj->AddComponent<MeshFilter>()
		->SetMesh(ResourceManager::Instance()->GetMesh(type));

	obj->AddComponent<MeshRenderer>();

	return obj;
}



void GameObject::SetActive(bool active)
{
	if (m_active == active) return;

	m_active = active;

	if (m_active)
	{
		for (auto& comp : m_components)
		{
			// 게임 오브젝트 활성화 시, 
			// 비활성 && 원래 체크된 컴포넌트만 활성화
			if (!comp->m_active && comp->m_enabled)
			{
				comp->m_active = true;
				comp->OnEnable();
			}
		}
	}
	else
	{
		// 게임 오브젝트 비활성화 시, 모든 활성 컴포넌트 비활성화
		for (auto& comp : m_components)
		{
			if (comp->m_active)
			{
				comp->m_active = false;
				comp->OnDisable();
			}
		}
	}
}

void GameObject::SetName(std::string& name)
{
	m_name = name;
}
