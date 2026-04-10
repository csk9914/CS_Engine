#include "EditorUI.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "RenderPipeline.h"

#include "GameView.h"
#include "SceneView.h"
#include "ProjectPanel.h"
#include "Gizmo.h"

#include "Scene.h"
#include "GeometryGenerator.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui/imgui_stdlib.h"

#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "MeshFilter.h"
#include "Rigidbody.h"
#include "GameApp.h"

#include "DX11Renderer.h"

#include "ComponentRegistry.h"
#include <map>

EditorUI::EditorUI()
{
}

EditorUI::~EditorUI() { Shutdown(); }

bool EditorUI::Init(HWND hWnd, DX11Renderer* renderer)
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
	ImGui_ImplDX11_Init(renderer->GetDevice(), renderer->GetContext());

	// Gizmo
	m_gizmo = std::make_unique<Gizmo>();
	m_gizmo->Init();

	std::string assetsRoot = "./Assets";

	// EditorWindow 등록 (SceneView가 Gizmo 렌더를 담당)
	m_windows.push_back(std::make_unique<SceneView>(this, renderer));
	m_windows.push_back(std::make_unique<GameView>(this, renderer));
	m_windows.push_back(std::make_unique<ProjectPanel>(this, renderer, assetsRoot));

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

void EditorUI::Update(float dt, RenderPipeline* pipeline)
{
	// 각 EditorWindow가 RenderPipeline에 렌더 요청을 예약
	for (auto& w : m_windows)
		w->Update(dt, pipeline);
}

void EditorUI::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	m_gizmo->BeginFrame();
}

void EditorUI::Draw(GameApp* gameApp, ID3D11ShaderResourceView* sceneSRV, ID3D11ShaderResourceView* gameSRV)
{
	DrawMenuBar();
	if (m_dockingEnabled) DrawMainDockSpace();

	RenderTopToolbar(gameApp);

	// EditorWindow::OnGUI() 호출 (SceneView, GameView)
	// ※ 중복 호출 제거 — 한 번만 호출
	for (auto& w : m_windows)
		w->OnGUI();

	// 공용 패널
	DrawHierarchy(gameApp->GetCurrentScene());
	DrawInspector();
}

void EditorUI::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


void EditorUI::RenderTopToolbar(GameApp* gameApp)
{
	// 상단 툴바 레이아웃 설정
	ImGui::BeginMainMenuBar();

	// 중앙 정렬을 위한 계산 (버튼 크기에 맞춰 조정 가능)
	float windowWidth = ImGui::GetWindowSize().x;
	float buttonWidth = 100.0f;
	ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

	bool isPlaying = gameApp->IsPlay();

	// 상태에 따라 버튼 텍스트와 색상 변경
	const char* label = isPlaying ? "Stop" : "Play";
	if (isPlaying)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f)); // 정지 버튼은 빨간색
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f)); // 플레이 버튼은 녹색
	}

	if (ImGui::Button(label, ImVec2(buttonWidth, 0)))
	{
		gameApp->SetIsPlay(!isPlaying);

		// [꿀팁] 플레이 버튼을 누를 때 포커스를 씬 뷰로 강제 이동하거나 
		// 마우스 입력을 게임용으로 전환하는 로직을 여기에 넣을 수 있어요.
	}

	ImGui::PopStyleColor();
	ImGui::EndMainMenuBar();
}

void EditorUI::DrawAddComponentButton()
{
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::Button("Add Component", ImVec2(-1.f, 30.f)))
	{
		ImGui::OpenPopup("AddComponentPopup");
	}

	if (ImGui::BeginPopup("AddComponentPopup"))
	{
		ImGui::TextDisabled("Search Components..."); // 나중에 검색 기능 넣기 좋음
		ImGui::Separator();

		// 1. 카테고리별로 그룹화 (Map 사용)
		// key: 카테고리 이름, value: 해당 카테고리에 속한 메타 데이터 포인터들
		std::map<std::string, std::vector<const ComponentMeta*>> categorized;

		auto& allComponents = ComponentRegistry::GetList();
		for (const auto& meta : allComponents)
		{
			categorized[meta.category].push_back(&meta);
		}

		// 2. 맵을 순회하며 메뉴 구성
		for (auto& pair : categorized)
		{
			const std::string& categoryName = pair.first;
			const auto& componentList = pair.second;

			// 카테고리 이름으로 서브 메뉴 생성
			if (ImGui::BeginMenu(categoryName.c_str()))
			{
				for (const auto* meta : componentList)
				{
					if (ImGui::Selectable(meta->name.c_str()))
					{
						meta->createFunc(m_selected);
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndMenu();
			}
		}
		ImGui::EndPopup();
	}
}

void EditorUI::LabeledDragFloat(const char* label, float* value, float speed)
{
	ImGui::Text(label);
	ImGui::SameLine(ImGui::GetWindowWidth() * 0.4f); // 창 너비의 40% 지점부터 시작
	ImGui::SetNextItemWidth(-1);
	std::string hiddenLabel = "##" + std::string(label);
	ImGui::DragFloat(hiddenLabel.c_str(), value, speed);
}

void EditorUI::LabeledDragFloat3(const char* label, float* value, float speed)
{
	ImGui::Text(label);
	ImGui::SameLine(ImGui::GetWindowWidth() * 0.4f); // 창 너비의 40% 지점부터 시작
	ImGui::SetNextItemWidth(-1);
	std::string hiddenLabel = "##" + std::string(label);
	ImGui::DragFloat3(hiddenLabel.c_str(), value, speed);
}

// ── DockSpace 
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

// ── 메뉴바 
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

// ── Hierarchy 
void EditorUI::DrawHierarchy(Scene* scene)
{
	ImGui::Begin("Hierarchy");
	if (!scene) { ImGui::End(); return; }

	const auto& objs = scene->GetGameObjects();
	GameObject* pendingDelete = nullptr;

	// 1. 오브젝트 리스트 루프
	for (int i = 0; i < (int)objs.size(); i++)
	{
		GameObject* obj = objs[i].get();
		if (!obj || obj->IsDestroy()) continue;

		std::string label = obj->GetName() + "##" + std::to_string(i);
		if (ImGui::Selectable(label.c_str(), m_selected == obj))
		{
			m_selected = obj;
		}

		// [A] 아이템 개별 우클릭 (Delete)
		// 괄호를 비워두면 방금 그린 Selectable에 자동으로 붙습니다.
		if (ImGui::BeginPopupContextItem())
		{
			m_selected = obj;
			if (ImGui::MenuItem("Delete Object"))
			{
				pendingDelete = obj;
				pendingDelete->Destroy();
			}
			ImGui::EndPopup();
		}
	}

	// 2. [핵심] 빈 공간 우클릭 처리 (우리가 방금 성공시킨 그 방식!)
	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !ImGui::IsAnyItemHovered())
	{
		ImGui::OpenPopup("HierarchyEmptyPopup");
	}

	if (ImGui::BeginPopup("HierarchyEmptyPopup"))
	{
		if (ImGui::MenuItem("Create Empty"))
			m_selected = scene->CreateGameObject("New GameObject");

		if (ImGui::BeginMenu("3D Object"))
		{
			if (ImGui::MenuItem("Plane"))
				m_selected = CreatePrimitive(PrimitiveType::Plane);

			if (ImGui::MenuItem("Cube"))
				m_selected = CreatePrimitive(PrimitiveType::Cube);

			if (ImGui::MenuItem("Sphere"))
				m_selected = CreatePrimitive(PrimitiveType::Sphere);

			if (ImGui::MenuItem("Capsule"))
				m_selected = CreatePrimitive(PrimitiveType::Capsule);

			ImGui::EndMenu();
		}


		ImGui::EndPopup();
	}

	// 3. 안전한 삭제 (루프 밖)
	if (pendingDelete)
	{
		if (m_selected == pendingDelete)
			m_selected = nullptr;

		//pendingDelete->OnDestroy();
		pendingDelete->GetScene()->ProcessDestroy();
	}

	// 4. 선택 해제 로직 (보호막)
	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		// 아이템 위가 아니고 + "HierarchyEmptyPopup"이 열려있지 않을 때만 해제
		if (!ImGui::IsAnyItemHovered() && !ImGui::IsPopupOpen("HierarchyEmptyPopup"))
		{
			m_selected = nullptr;
		}
	}

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

	// 1. Transform은 가장 중요하므로 명시적으로 먼저 그림
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		m_selected->GetTransform()->OnEditorGUI();
	}

	// 2. 나머지 모든 컴포넌트 자동 렌더링
	auto& components = m_selected->GetComponents();
	// 삭제할 컴포넌트를 임시로 담아둘 포인터 (루프 도중 삭제 방지)
	Component* pendingDelete = nullptr;

	for (auto& comp : components)
	{
		if (dynamic_cast<Transform*>(comp.get())) continue;

		ImGui::PushID(comp.get());

		bool open = ImGui::CollapsingHeader(comp->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		// 헤더를 우클릭했을 때 팝업 출력
		if (ImGui::BeginPopupContextItem("ComponentEditPopup"))
		{
			if (ImGui::MenuItem("Remove Component"))
			{
				pendingDelete = comp.get();
			}
			ImGui::EndPopup();
		}

		if (open) comp->OnEditorGUI();

		ImGui::PopID();
	}

	// 루프가 끝난 뒤 안전하게 삭제 처리
	if (pendingDelete)
	{
		m_selected->RemoveComponent(pendingDelete);
	}


	DrawAddComponentButton();
	ImGui::End();
}
