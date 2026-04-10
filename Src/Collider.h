#pragma once
#include "Component.h"
#include "Vector3.h"

enum class ColliderType 
{ 
	Box, 
	Sphere, 
	Capsule,
	Size,
};

class Collider;

struct CollisionInfo
{
	bool isColliding = false; // 충돌 여부
	float penetration = 0.0f; // 겹친 깊이 (얼마나 밀어내야 하는가?)

	Vector3 normal;           // 충돌 법선 (나를 어디로 밀어내야 하는가?)
	Vector3 contactPoint;     // 충돌 지점 (이펙트 생성 등에 사용)

	Collider* mine = nullptr;  // 내 콜라이더
	Collider* other = nullptr; // 상대 콜라이더
};

class Collider : public Component
{
public:
	Collider(ColliderType type, const std::string& name = "Collider");
	virtual ~Collider() = default;

	virtual void Awake() override;
	virtual void OnDestroy() override;
	ColliderType GetColliderType() const { return m_colliderType; }

	// 콜라이더의 중심점 (Transform 위치 + 오프셋)
	Vector3 GetCenter();

	virtual void AutoFit() = 0;
	virtual void OnEditorGUI() override;

public:
	bool IsTrigger() const { return m_isTrigger; }
	void SetTrigger(bool trigger) { m_isTrigger = trigger; }

protected:
	ColliderType m_colliderType;
	Vector3 m_offset = {};
	bool m_isTrigger;
};