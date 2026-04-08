#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"
#include "CollisionManager.h"

Collider::Collider(ColliderType type) : m_colliderType(type)
{
    CollisionManager::Instance()->AddCollider(this);
}

Vector3 Collider::GetCenter()
{
    return GetOwner()->GetTransform()->GetPosition() + m_offset;
}
