#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"
#include "CollisionManager.h"

Collider::Collider(ColliderType type, const std::string& name) : m_colliderType(type), Component(name)
{
    CollisionManager::Instance()->AddCollider(this);
}

Vector3 Collider::GetCenter()
{
    return GetGameObject()->GetTransform()->GetPosition() + m_offset;
}
