#pragma once
#include "Component.h"
#include "Transform.h"

#include <vector>
#include <memory>
#include <string>

#include "Object.h"
#include "PrimitiveType.h"

class GameObject : public Object
{
public:
	GameObject(const std::string& name = "GameObject");
	virtual ~GameObject() {}

	// 객체가 파괴될 때 자원 정리
	virtual void OnDestroy();

	// 객체를 제거 목록에 추가 (큐에 넣는 방식으로 실제 제거는 나중에 처리)
	void Destroy() { m_isDestroyed = true; }
	bool IsDestroy() const { return m_isDestroyed; }

public:
	void Update(float deltaTime);

	// AddComponent 템플릿 함수 (HEAD에 있던 내용)
	template<typename T>
	T* AddComponent()
	{
		auto component = std::make_unique<T>();
		T* ptr = component.get();

		ptr->m_gameObject = this;
		ptr->Awake();

		if (this->GetActive() && ptr->m_enabled)
		{
			ptr->SetActive(true);
			ptr->OnEnable();
		}

		m_components.push_back(std::move(component));
		return ptr;
	}


	// 다이나믹 캐스팅에서 Tag로 변경할 수도 있음
	template<typename T>
	T* GetComponent()
	{
		for (auto& comp : m_components)
		{
			if (T* t = dynamic_cast<T*>(comp.get()))
				return t;
		}
		return nullptr;
	}

	// 모든 컴포넌트 리스트를 반환 (인스펙터 루프용)
	const std::vector<std::unique_ptr<Component>>& GetComponents() const { return m_components; }
	std::vector<std::unique_ptr<Component>>& GetComponents() { return m_components; }

	// 특정 타입의 컴포넌트들을 모두 찾아야 할 때 (유니티의 GetComponents<T>와 유사)
	template<typename T>
	std::vector<T*> GetComponents()
	{
		std::vector<T*> results;
		for (auto& comp : m_components)
		{
			if (T* t = dynamic_cast<T*>(comp.get()))
				results.push_back(t);
		}
		return results;
	}

	// 1. 특정 컴포넌트 인스턴스 제거 (인스펙터 등에서 주소값으로 제거할 때)
	void RemoveComponent(Component* component);

	// 2. 특정 타입의 컴포넌트 제거 (유니티 스타일: RemoveComponent<MeshRenderer>())
	template<typename T>
	void RemoveComponent()
	{
		for (auto it = m_components.begin(); it != m_components.end(); ++it)
		{
			if (dynamic_cast<T*>(it->get()))
			{
				(*it)->OnDestroy(); // 자원 정리 호출
				m_components.erase(it);
				return; // 하나만 지우고 종료
			}
		}
	}

	static GameObject* CreateGameObject(const std::string& name = "GameObject");
	static GameObject* CreatePrimitive(PrimitiveType type);

public:

	virtual void SetActive(bool active) override;

	// Transform
	Transform* GetTransform() const { return m_transform; }
	Transform* GetTransform()  { return m_transform; }

private:


	bool m_isDestroyed = false;

	Transform* m_transform = nullptr;
	std::vector<std::unique_ptr<Component>> m_components;


};

// 클래스 외부(전역 공간)에 선언
inline GameObject* CreateGameObject(const std::string& name = "GameObject")
{
	return GameObject::CreateGameObject(name);
}

inline GameObject* CreatePrimitive(PrimitiveType type)
{
	return GameObject::CreatePrimitive(type);
}