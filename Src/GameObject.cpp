#include "GameObject.h"
#include "SceneManager.h"
#include "PrimitiveType.h"
#include "ResourceManager.h"

#include "MeshData.h"
#include "Mesh.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"

#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"

#include "Scene.h"

GameObject::GameObject(Scene* scene, const std::string& name) : Object(name, true),m_scene(scene), m_isDestroyed(false)
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

void GameObject::Awake()
{
	// 이미 한 번 실행했다면...
	if (m_isAwake) return;

	m_isAwake = true;
	for (auto& comp : m_components)
	{
		comp->Awake();
	}

	// 2단계: Awake가 끝난 후, 활성화 상태라면 OnEnable 호출
	if (this->GetActive())
	{
		for (auto& comp : m_components)
		{
			if (comp->m_enabled)
			{
				comp->SetActive(true); // 컴포넌트 내부 상태 활성화
				comp->OnEnable();      // 이벤트 호출
			}
		}
	}
}

void GameObject::Start()
{
	for (auto& comp : m_components)
	{
		comp->Start();
	}
}

void GameObject::FixedUpdate(float deltaTime)
{
	for (auto& comp : m_components)
	{
		// 활성화된 컴포넌트만 Update() 호출
		if (comp->m_enabled)
		{
			comp->FixedUpdate(deltaTime);
		}
	}
}

void GameObject::Update(float deltaTime)
{
	if (!GetActive()) return;

	for (auto& comp : m_components)
	{
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

	switch (type)
	{
	case PrimitiveType::Plane:
		obj->AddComponent<BoxCollider>()->AutoFit();
		obj->GetComponent<BoxCollider>()->SetSize(obj->GetComponent<BoxCollider>()->GetSize() + Vector3{0, 0.1f, 0});
		break;
	case PrimitiveType::Cube:
		obj->AddComponent<BoxCollider>()->AutoFit();
		break;
	case PrimitiveType::Sphere:
		obj->AddComponent<SphereCollider>()->AutoFit();
		break;
	case PrimitiveType::Capsule:
		obj->AddComponent<CapsuleCollider>()->AutoFit();
		break;
	default:
		break;
	}

	return obj;
}

void GameObject::SetActive(bool active)
{
	if (GetActive() == active) return;

	Object::SetActive(active);


	for (auto& comp : m_components)
	{
		// 켜질 때는 m_enabled여야 하고, 꺼질 때는 무조건 실행
		bool resultActive = active && comp->m_enabled;

		// 현재 컴포넌트 상태와 타겟 상태가 다를 때만 이벤트 발생
		if (comp->GetActive() != resultActive)
		{
			comp->SetActive(resultActive);

			if (resultActive) 
				comp->OnEnable();
			else 
				comp->OnDisable();
		}
	}
}

bool GameObject::IsPlaying() const
{
	return m_scene->IsPlaying();
}
