#pragma once

class GameObject;

class Component
{
	friend class GameObject;

protected:
	Component() : m_active(true), m_enabled(true), m_started(false), m_owner(nullptr) {}

public:
	virtual ~Component() {}

public:
	virtual void Awake() {}   // 생성 직후
	virtual void Start() {}   // 첫 업데이트 전
	virtual void Update(float dt) {}
	virtual void LateUpdate(float dt) {}
	virtual void OnEnable() {}
	virtual void OnDisable() {}
	virtual void OnDestroy() {}

public:
	void SetEnabled(bool enabled);

public:
	GameObject* GetOwner() { return m_owner; }
	const GameObject* GetOwner() const { return m_owner; }

private:
	bool m_active;
	bool m_enabled;
	bool m_started;
	GameObject* m_owner;
};