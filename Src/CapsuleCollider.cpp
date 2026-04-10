#include "CapsuleCollider.h"
#include "EditorUI.h"
#include "MeshFilter.h"
#include "GameObject.h"

#include "ComponentRegistry.h"

void CapsuleCollider::AutoFit()
{
	auto filter = gameObject()->GetComponent<MeshFilter>();
	if (!filter || !filter->GetMesh()) return;

	DirectX::XMFLOAT3 min, max;
	filter->GetMesh()->GetBounds(min, max);

	Vector3 size = { max.x - min.x, max.y - min.y, max.z - min.z };

	// 1. 중심점 설정
	m_offset = { (max.x + min.x) * 0.5f, (max.y + min.y) * 0.5f, (max.z + min.z) * 0.5f };

	// 2. 높이와 반지름 설정 (Y축 기준)
	SetHeight(size.y);
	// 반지름은 X와 Z 중 더 넓은 쪽을 기준으로 잡음
	m_radius = (std::max)(size.x, size.z) * 0.5f;

}

void CapsuleCollider::OnEditorGUI()
{
	Collider::OnEditorGUI();

	EditorUI::LabeledDragFloat("Radius", &m_radius);
	EditorUI::LabeledDragFloat("Height", &m_height);
}

void CapsuleCollider::GetSegment(Vector3& outTop, Vector3& outBottom)
{
	// 1. 현재 오브젝트의 월드 위치와 회전 정보를 가져옵니다.
	Vector3 center = GetCenter();

	// 2. 캡슐의 방향 (일반적으로 로컬 Y축을 사용)
	Vector3 up = { 0, 1,0 };

	// 3. 선분은 중심에서 위아래로 (전체 높이 /2)만큼 떨어진 지점
	// 실제 캡슐 높이는 원통 높이 + 2 * 반지름
	float halfHeight = m_height * 0.5f;

	outTop = center + (up * halfHeight);
	outBottom = center - (up * halfHeight);

}

REGISTER_COMPONENT(CapsuleCollider, "Physics")
