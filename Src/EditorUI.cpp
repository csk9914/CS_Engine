#include "EditorUI.h"
#include "GameScene.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "RenderPipeline.h"
#include "GameView.h"
#include "SceneView.h"
#include "Gizmo.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui/imgui_stdlib.h"

EditorUI::EditorUI()
{
}

EditorUI::~EditorUI() { Shutdown(); }

bool EditorUI::Init(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	m_dockingEnabled = true;

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 4.f;
	style.FrameRounding = 3.f;
	style.FramePadding = { 6.f, 3.f };
	style.ItemSpacing = { 8.f, 5.f };
	style.WindowBorderSize = 1.f;

	ImVec4* c = style.Colors;
	c[ImGuiCol_WindowBg] = { 0.22f,0.22f,0.22f,1.f };
	c[ImGuiCol_ChildBg] = { 0.18f,0.18f,0.18f,1.f };
	c[ImGuiCol_Header] = { 0.31f,0.47f,0.66f,1.f };
	c[ImGuiCol_HeaderHovered] = { 0.40f,0.57f,0.76f,1.f };
	c[ImGuiCol_HeaderActive] = { 0.26f,0.59f,0.98f,1.f };
	c[ImGuiCol_Button] = { 0.31f,0.31f,0.31f,1.f };
	c[ImGuiCol_ButtonHovered] = { 0.40f,0.40f,0.40f,1.f };
	c[ImGuiCol_FrameBg] = { 0.16f,0.16f,0.16f,1.f };
	c[ImGuiCol_TitleBg] = { 0.16f,0.16f,0.16f,1.f };
	c[ImGuiCol_TitleBgActive] = { 0.20f,0.20f,0.20f,1.f };
	c[ImGuiCol_Tab] = { 0.20f,0.20f,0.20f,1.f };
	c[ImGuiCol_TabHovered] = { 0.35f,0.35f,0.35f,1.f };
	c[ImGuiCol_TabActive] = { 0.28f,0.28f,0.28f,1.f };
	c[ImGuiCol_DockingPreview] = { 0.26f,0.59f,0.98f,0.7f };
	c[ImGuiCol_Separator] = { 0.30f,0.30f,0.30f,1.f };

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device, context);

	// Gizmo
	m_gizmo = std::make_unique<Gizmo>();
	m_gizmo->Init();

	// EditorWindow 등록 (SceneView가 Gizmo 렌더를 담당)
	m_windows.push_back(std::make_unique<SceneView>());
	m_windows.push_back(std::make_unique<GameView>());

	for (auto& w : m_windows) 
		w->Init();

	m_initialized = true;
	return true;
}

void EditorUI::Shutdown()
{
	if (!m_initialized) return;
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	m_initialized = false;
}

void EditorUI::Update(float dt, RenderPipeline* pipeline, DX11Renderer* renderer)
{
	// 각 EditorWindow가 RenderPipeline에 렌더 요청을 예약
	for (auto& w : m_windows)
		w->Update(dt, pipeline, renderer);
}

void EditorUI::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	m_gizmo->BeginFrame();
}

void EditorUI::Draw(GameScene* scene,
	ID3D11ShaderResourceView* sceneSRV,
	ID3D11ShaderResourceView* gameSRV)
{
	DrawMenuBar();
	if (m_dockingEnabled) DrawMainDockSpace();

	// EditorWindow::OnGUI() 호출 (SceneView, GameView)
	// ※ 중복 호출 제거 — 한 번만 호출
	for (auto& w : m_windows)
		w->OnGUI();

	// 공용 패널
	DrawHierarchy(scene);
	DrawInspector();
}

void EditorUI::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

// ── DockSpace ─────────────────────────────────────────────────────────
void EditorUI::DrawMainDockSpace()
{
	ImGuiViewport* vp = ImGui::GetMainViewport();
	float menuH = ImGui::GetFrameHeight();
	ImGui::SetNextWindowPos({ vp->Pos.x, vp->Pos.y + menuH });
	ImGui::SetNextWindowSize({ vp->Size.x, vp->Size.y - menuH });
	ImGui::SetNextWindowViewport(vp->ID);

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::Begin("##DockSpace", nullptr, flags);
	ImGui::PopStyleVar(2);
	ImGui::DockSpace(ImGui::GetID("MainDockSpace"), { 0,0 },
		ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();
}

// ── 메뉴바 ────────────────────────────────────────────────────────────
void EditorUI::DrawMenuBar()
{
	if (!ImGui::BeginMainMenuBar()) return;

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Exit")) PostQuitMessage(0);
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("GameObject"))
	{
		ImGui::MenuItem("Create Empty"); // TODO
		ImGui::MenuItem("Create Cube");  // TODO
		ImGui::EndMenu();
	}

	char fps[32];
	snprintf(fps, sizeof(fps), "%.1f FPS", ImGui::GetIO().Framerate);
	float w = ImGui::CalcTextSize(fps).x + 16.f;
	ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - w + ImGui::GetCursorPosX());
	ImGui::TextDisabled("%s", fps);

	ImGui::EndMainMenuBar();
}

// ── Hierarchy ─────────────────────────────────────────────────────────
void EditorUI::DrawHierarchy(GameScene* scene)
{
	ImGui::Begin("Hierarchy");
	if (!scene) { ImGui::TextDisabled("No scene"); ImGui::End(); return; }

	const auto& objs = scene->GetGameObjects();
	for (int i = 0; i < (int)objs.size(); i++)
	{
		GameObject* obj = objs[i].get();
		if (!obj) continue;

		bool selected = (m_selected == obj);
		if (!obj->GetActive())
			ImGui::PushStyleColor(ImGuiCol_Text, { 0.5f,0.5f,0.5f,1.f });

		std::string label = obj->GetName();
		if (label.empty()) label = "GameObject_" + std::to_string(i);

		if (ImGui::Selectable(label.c_str(), selected))
			m_selected = obj;

		if (ImGui::BeginPopupContextItem())
		{
			m_selected = obj;
			if (ImGui::MenuItem("Delete"))   obj->Destroy();
			if (ImGui::MenuItem(obj->GetActive() ? "Deactivate" : "Activate"))
				obj->SetActive(!obj->GetActive());
			ImGui::EndPopup();
		}

		if (!obj->GetActive()) ImGui::PopStyleColor();
	}

	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
		m_selected = nullptr;

	ImGui::End();
}

// ── Inspector ─────────────────────────────────────────────────────────
void EditorUI::DrawInspector()
{
	ImGui::Begin("Inspector");
	if (!m_selected) { ImGui::TextDisabled("No object selected"); ImGui::End(); return; }

	bool active = m_selected->GetActive();
	if (ImGui::Checkbox("##active", &active)) m_selected->SetActive(active);
	ImGui::SameLine();

	std::string name = m_selected->GetName();
	ImGui::SetNextItemWidth(-1.f);
	if (ImGui::InputText("##name", &name, ImGuiInputTextFlags_EnterReturnsTrue))
		m_selected->SetName(name);
	ImGui::Separator();

	// Transform
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Transform* tr = m_selected->GetTransform();
		if (tr)
		{
			Vector3 pos = tr->GetPosition();
			Vector3 rot = tr->GetRotation();
			Vector3 scl = tr->GetScale();
			ImGui::Text("Position"); ImGui::SetNextItemWidth(-1.f);
			if (ImGui::DragFloat3("##pos", &pos.x, 0.1f)) tr->SetPosition(pos);
			ImGui::Text("Rotation"); ImGui::SetNextItemWidth(-1.f);
			if (ImGui::DragFloat3("##rot", &rot.x, 1.f))  tr->SetRotation(rot);
			ImGui::Text("Scale");    ImGui::SetNextItemWidth(-1.f);
			if (ImGui::DragFloat3("##scl", &scl.x, 0.1f, 0.001f, 1000.f)) tr->SetScale(scl);
		}
	}

	// Camera
	Camera* cam = m_selected->GetComponent<Camera>();
	if (cam && ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat("FOV", &cam->m_fov, 0.5f, 10.f, 170.f);
		ImGui::DragFloat("Near", &cam->m_nearZ, 0.01f, 0.01f, 10.f);
		ImGui::DragFloat("Far", &cam->m_farZ, 1.f, 10.f, 10000.f);
		ImGui::DragFloat("Move Speed", &cam->m_moveSpeed, 0.5f, 0.1f, 100.f);
		ImGui::DragFloat("Rot Speed", &cam->m_rotateSpeed, 0.5f, 1.f, 180.f);
		ImGui::DragFloat("Zoom Speed", &cam->m_zoomSpeed, 0.5f, 1.f, 100.f);
	}

	// MeshRenderer
	MeshRenderer* mr = m_selected->GetComponent<MeshRenderer>();
	if (mr && ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float col[4] = { mr->GetColor().x, mr->GetColor().y,
						 mr->GetColor().z, mr->GetColor().w };
		if (ImGui::ColorEdit4("Color", col))
			mr->SetColor(col[0], col[1], col[2], col[3]);
	}

	ImGui::End();
}
