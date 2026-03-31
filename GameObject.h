#pragma once
#include "Component.h"

#include <vector>
#include <memory>

#include "Transform.h"
class GameObject
{
public:
	GameObject();
	virtual ~GameObject() {}

	// 객체가 파괴될 때 자원 정리
	virtual void OnDestroy();

	// 객체를 제거 목록에 추가 (큐에 넣는 방식으로 실제 제거는 나중에 처리)
	void Destroy() { m_isDestroyed = true; }
	bool IsDestroy() const { return m_isDestroyed; }

public:

<<<<<<< HEAD
	template<typename T>
	T* AddComponent()
=======

protected:
	GameObject(LoaderParams* params) : m_active(true), m_isDestroyed(false)
>>>>>>> main
	{
		auto component = std::make_unique<T>();
		T* ptr = component.get();

		ptr->m_owner = this;
		ptr->Awake(); // 컴포넌트가 추가될 때 Awake() 호출

		// 오브젝트가 활성화 상태이고, 컴포넌트가 활성화 가능하다면, 컴포넌트 활성화
		if (this->m_active && ptr->m_enabled)
		{
			ptr->m_active = true;
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

	void Update(float deltaTime);

public:
	bool GetActive() const { return m_active; }
	virtual void SetActive(bool active);

	Transform* GetTransform() const { return m_transform; }
	Transform* GetTransform()  { return m_transform; }

private:
	bool m_active;
	bool m_isDestroyed;
	Transform* m_transform;
	std::vector<std::unique_ptr<Component>> m_components;


};