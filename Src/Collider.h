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

class Collider : public Component
{
public:
	Collider(ColliderType type, const std::string& name = "Collider");
	virtual ~Collider() = default;

	ColliderType GetColliderType() const { return m_colliderType; }

	// 콜라이더의 중심점 (Transform 위치 + 오프셋)
	Vector3 GetCenter();

protected:
	ColliderType m_colliderType;
	Vector3 m_offset = {};
};