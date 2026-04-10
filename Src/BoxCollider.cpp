#include "BoxCollider.h"
#include "GameObject.h"
#include "EditorUI.h"

#include "MeshFilter.h"

#include "ComponentRegistry.h"

BoxCollider::~BoxCollider() {}

void BoxCollider::OnEditorGUI()
{
	Collider::OnEditorGUI();

	EditorUI::LabeledDragFloat3("Size", &m_size.x);
}

void BoxCollider::AutoFit()
{
	// 1. 같은 오브젝트의 MeshFilter 찾기
	auto filter = gameObject()->GetComponent<MeshFilter>();
	if (!filter || !filter->GetMesh()) return;

	// 2. 메쉬에서 바운딩 정보 가져오기
	DirectX::XMFLOAT3 min, max;
	filter->GetMesh()->GetBounds(min, max);

	// 3. 크기와 오프셋 계산
	// Size = max - min
	m_size = { max.x - min.x, max.y - min.y, max.z - min.z };

	// Center = (max + min) * 0.5
	m_offset = { (max.x + min.x) * 0.5f, (max.y + min.y) * 0.5f, (max.z + min.z) * 0.5f };


}

Vector3 BoxCollider::GetMin()
{
	Vector3 center = GetCenter();
	Vector3 scale = gameObject()->GetTransform()->GetScale();

	// (중심) - (크기 * 스케일 * 0.5)
	return center - Vector3(m_size.x * scale.x, m_size.y * scale.y, m_size.z * scale.z) * 0.5f;
}

Vector3 BoxCollider::GetMax()
{
	Vector3 center = GetCenter();
	Vector3 scale = gameObject()->GetTransform()->GetScale();

	// (중심) + (크기 * 스케일 * 0.5)
	return center + Vector3(m_size.x * scale.x, m_size.y * scale.y, m_size.z * scale.z) * 0.5f;
}


REGISTER_COMPONENT(BoxCollider, "Physics")