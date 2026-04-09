#include "MeshFilter.h"
#include "imgui/imgui.h"
#include "Object.h"

#include "Collider.h"
#include "GameObject.h"
void MeshFilter::SetMesh(Mesh* mesh) 
{ 
	m_mesh = mesh; 

	auto col = GetGameObject()->GetComponent<Collider>();
	if (col)
	{
		col->AutoFit();
	}
}

void MeshFilter::OnEditorGUI()
{
	ImGui::Text("Mesh Asset:");
	ImGui::SameLine();

	// 현재 장착된 메쉬의 이름 표시
	std::string meshName = m_mesh ? m_mesh->GetName() : "None (Mesh)";

	// 버튼 형태로 표시하여 클릭 시 메쉬 선택 창을 띄울 수 있게 함
	if (ImGui::Button(meshName.c_str(), ImVec2(ImGui::GetContentRegionAvail().x - 30, 0)))
	{
		// TODO: 메쉬 선택 팝업이나 리소스 브라우저 오픈 로직
	}

	// 마우스를 올렸을 때 상세 정보 표시 (Tooltip)
	if (m_mesh && ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("Index Count: %u", m_mesh->GetIndexCount());
		// Mesh 클래스에 VertexCount 등이 있다면 추가 표시
		ImGui::EndTooltip();
	}

	// 드래그 앤 드롭 타겟 (에셋 창에서 메쉬를 끌어다 놓을 때)
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_MESH"))
		{
			// 로직: 페이로드에서 받은 경로로 새로운 메쉬 로드 및 SetMesh
			// Mesh* newMesh = (Mesh*)payload->Data;
			// SetMesh(newMesh);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("X")) // 메쉬 해제 버튼
	{
		SetMesh(nullptr);
	}
}
