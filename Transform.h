#pragma once
#include "Component.h"
#include "Vector2.h"

class Transform : public Component
{
public:
	Transform() : m_position(0, 0), m_rotation(0, 0), m_scale(1.f, 1.f) {}
	virtual ~Transform() {}

public:
	//virtual void Awake() override {}
	//virtual void Start() override {}
	//virtual void Update(float dt) override  {}
	//virtual void LateUpdate(float dt) override {}
	//virtual void OnEnable() override  {}
	//virtual void OnDisable() override  {}
	//virtual void OnDestroy() override  {}

public:
	const Vector2& GetPosition() const { return m_position; }
	const Vector2& GetRotation() const { return m_rotation; }
	const Vector2& GetScale() const { return m_scale; }
	void SetPosition(const Vector2& position) { m_position = position; }
	void SetRotation(const Vector2& rotation) { m_rotation = rotation; }
	void SetScale(const Vector2& scale) { m_scale = scale; }

private:
	Vector2 m_position;
	Vector2 m_rotation;
	Vector2 m_scale;

};