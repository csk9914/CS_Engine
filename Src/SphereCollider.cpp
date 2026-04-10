#include "SphereCollider.h"
#include "EditorUI.h"
#include "MeshFilter.h"
#include "GameObject.h"

#include "ComponentRegistry.h"

void SphereCollider::AutoFit()
{
    auto filter = gameObject()->GetComponent<MeshFilter>();
    if (!filter || !filter->GetMesh()) return;

    DirectX::XMFLOAT3 min, max;
    filter->GetMesh()->GetBounds(min, max);

    Vector3 size = { max.x - min.x, max.y - min.y, max.z - min.z };

    // 1. 중심점 설정
    m_offset = { (max.x + min.x) * 0.5f, (max.y + min.y) * 0.5f, (max.z + min.z) * 0.5f };

    // 2. 반지름 설정: 가로, 세로, 높이 중 가장 큰 값의 절반
    float maxSide = (std::max)({ size.x, size.y, size.z });
    m_radius = (maxSide * 0.5f);
}

void SphereCollider::OnEditorGUI()
{
    Collider::OnEditorGUI();

	EditorUI::LabeledDragFloat("Radius", &m_radius);
}

REGISTER_COMPONENT(SphereCollider, "Physics")