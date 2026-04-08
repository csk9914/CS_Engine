#include "BoxCollider.h"
#include "GameObject.h"

Vector3 BoxCollider::GetMin()
{
	Vector3 center = GetCenter();
	Vector3 scale = GetOwner()->GetTransform()->GetScale();

	// (중심) - (크기 * 스케일 * 0.5)
	return center - Vector3(m_size.x * scale.x, m_size.y * scale.y, m_size.z * scale.z) * 0.5f;
}

Vector3 BoxCollider::GetMax()
{
	Vector3 center = GetCenter();
	Vector3 scale = GetOwner()->GetTransform()->GetScale();

	// (중심) + (크기 * 스케일 * 0.5)
	return center + Vector3(m_size.x * scale.x, m_size.y * scale.y, m_size.z * scale.z) * 0.5f;
}
