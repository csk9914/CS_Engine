#include "GameObject.h"
#include "SceneManager.h"
#include "PrimitiveType.h"
#include "ResourceManager.h"

#include "MeshData.h"
#include "Mesh.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"

GameObject::GameObject(const std::string& name) : Object(name, true), m_isDestroyed(false)
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
	if (!GetActive()) return;

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

void GameObject::RemoveComponent(Component* component)
{
	if (component == nullptr) return;

	// Transform은 게임오브젝트와 운명을 같이하므로 삭제 방지 (안전장치)
	if (component == m_transform) return;

	auto it = std::find_if(m_components.begin(), m_components.end(),
		[component](const std::unique_ptr<Component>& c)
		{
			return c.get() == component;
		});

	if (it != m_components.end())
	{
		// 1. 컴포넌트의 종료 루틴 호출 (필요시)
		if ((*it)->GetActive())
			(*it)->OnDisable();

		(*it)->OnDestroy();

		// 2. 실제 메모리 해제 및 벡터에서 제거
		m_components.erase(it);
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
	if (GetActive() == active) return;

	Object::SetActive(active);

	if (GetActive())
	{
		for (auto& comp : m_components)
		{
			// 게임 오브젝트 활성화 시, 
			// 비활성 && 원래 체크된 컴포넌트만 활성화
			if (!comp->GetActive() && comp->m_enabled)
			{
				comp->SetActive(true);
				comp->OnEnable();
			}
		}
	}
	else
	{
		// 게임 오브젝트 비활성화 시, 모든 활성 컴포넌트 비활성화
		for (auto& comp : m_components)
		{
			if (comp->GetActive())
			{
				comp->SetActive(false);
				comp->OnDisable();
			}
		}
	}
}
