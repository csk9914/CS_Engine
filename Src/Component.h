#pragma once
#include "Object.h"

class GameObject;

class Component : public Object
{
	friend class GameObject;

protected:
	Component(const std::string& name = "Component", bool useIndexing = false) : Object(name, useIndexing), m_enabled(true), m_gameObject(nullptr) {}

public:
	virtual ~Component() {}

public:
	virtual void Awake() {}   // 생성 직후
	virtual void Start() {}   // 첫 업데이트 전
	virtual void FixedUpdate(float dt) {}
	virtual void Update(float dt) {}
	virtual void LateUpdate(float dt) {}
	virtual void OnEnable() {}
	virtual void OnDisable() {}
	virtual void OnDestroy() {}

public:
	virtual void OnEditorGUI() {}


public:
	void SetEnabled(bool enabled);
	bool IsEnabled() const { return m_enabled; }

public:
	GameObject* gameObject() { return m_gameObject; }
	const GameObject* gameObject() const { return m_gameObject; }

private:
	bool m_enabled;
	//bool m_started;
	GameObject* m_gameObject;
};