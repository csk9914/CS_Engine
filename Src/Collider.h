#pragma once
#include "Component.h"
#include "Vector3.h"


/*
IsTrigger 처리: 
info.isColliding이 true라도, 한쪽이 IsTrigger라면 
ResolveCollision(밀어내기)을 호출하지 않고 OnTriggerEnter 이벤트만 발생시켜야 합니다.

Slop (여유값): 
아주 미세하게 겹쳤을 때는 밀어내지 않아야 
파르르 떨리는 진동(Jittering)이 없습니다. 보통 0.01f 정도의 여유를 둡니다.

Static vs Dynamic: 
Rigidbody가 없는 콜라이더는 유니티에서 'Static Collider'로 취급됩니다. 
이들은 절대 밀려나지 않으며, 벽이나 바닥 역할을 합니다.
*/

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