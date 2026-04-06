#pragma once
#include "Component.h"
#include "Transform.h"

#include <vector>
#include <memory>
#include <string>

class GameObject
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

	// AddComponent 템플릿 함수 (HEAD에 있던 내용)
	template<typename T>
	T* AddComponent()
	{
		auto component = std::make_unique<T>();
		T* ptr = component.get();

		ptr->m_owner = this;
		ptr->Awake();

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
	// 활성화
	bool GetActive() const { return m_active; }
	virtual void SetActive(bool active);

	// Transform
	Transform* GetTransform() const { return m_transform; }
	Transform* GetTransform()  { return m_transform; }

	// name
	const std::string& GetName() const { return m_name; }
	void SetName(std::string& name);

private:
	std::string m_name;
	bool m_active = true;
	bool m_isDestroyed = false;

	Transform* m_transform = nullptr;
	std::vector<std::unique_ptr<Component>> m_components;


};