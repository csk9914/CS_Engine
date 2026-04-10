#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"
#include "CollisionManager.h"

#include "EditorUI.h"

Collider::Collider(ColliderType type, const std::string& name) : Component(name), m_colliderType(type),  m_isTrigger(false) 
{
}

void Collider::Awake()
{
	CollisionManager::Instance()->AddCollider(this);
}

void Collider::OnDestroy()
{
	CollisionManager::Instance()->RemoveCollider(this);
}

Vector3 Collider::GetCenter()
{
	return gameObject()->GetTransform()->GetPosition() + m_offset;
}

void Collider::OnEditorGUI()
{
	ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 버튼 아래 살짝 여백 ㅡㅡ+🧪

	float width = ImGui::GetContentRegionAvail().x; // 현재 사용 가능한 가로 너비
	float margin = 20.0f;

	// 1. 자동 맞춤 버튼 (가장 잘 보이는 상단에 배치)
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (margin * 0.5f)); // 왼쪽 여백 시작
	if (ImGui::Button("Auto Fit to Mesh", ImVec2(width - margin, 30.0f))) // 가로 꽉 차게
	{
		AutoFit(); // 자식이 override한 함수가 호출됨!
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 버튼 아래 살짝 여백 ㅡㅡ+🧪

	EditorUI::LabeledDragFloat3("Center Offset", &m_offset.x);

	ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 간격 벌리기

	// 3. Is Trigger (양끝 배치 또는 여백 주기)
		// 텍스트는 왼쪽에, 체크박스는 오른쪽에 두고 싶을 때의 팁입니다.
	ImGui::Text("IsTrigger");
	ImGui::SameLine(width - 30.0f); // 오른쪽 끝에서 30픽셀 앞으로 커서 이동
	ImGui::Checkbox("##IsTrigger", &m_isTrigger); // ##을 붙이면 텍스트 없이 박스만 나옴

	ImGui::Separator(); // 공통 영역과 고유 영역 구분선
}
