#include "EditorUI.h"
#include "GameScene.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui/imgui_stdlib.h"

EditorUI::~EditorUI()
{
    Shutdown();
}

bool EditorUI::Init(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    // Docking 지원 여부 확인 후 활성화
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

#ifdef IMGUI_HAS_DOCK
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    m_dockingEnabled = true;
#else
    // docking 브랜치면 아래 플래그가 정의되어 있음
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable ||
        ImGui::GetIO().BackendFlags)
    {
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        m_dockingEnabled = true;
    }
#endif

    // Unity 다크 테마
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 4.f;
    style.FrameRounding = 3.f;
    style.FramePadding = { 6.f, 3.f };
    style.ItemSpacing = { 8.f, 5.f };
    style.WindowBorderSize = 1.f;
    style.ChildBorderSize = 1.f;

    // Unity 색상 팔레트 (근사값)
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.22f, 0.22f, 0.22f, 1.f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.f);
    colors[ImGuiCol_Header] = ImVec4(0.31f, 0.47f, 0.66f, 1.f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.40f, 0.57f, 0.76f, 1.f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.f);
    colors[ImGuiCol_Button] = ImVec4(0.31f, 0.31f, 0.31f, 1.f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.22f, 1.f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.f);
    colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 1.f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.f);
    colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.7f);
    colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.30f, 1.f);

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(device, context);

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

void EditorUI::BeginFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void EditorUI::Draw(GameScene* scene)
{
    DrawMenuBar();

    if (m_dockingEnabled)
        DrawMainDockSpace();

    DrawHierarchy(scene);
    DrawInspector();
}

void EditorUI::EndFrame()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

// ── DockSpace (Unity 레이아웃) ─────────────────────────────────────────
void EditorUI::DrawMainDockSpace()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    // 메뉴바 높이만큼 아래로 내림
    float menuBarHeight = ImGui::GetFrameHeight();
    ImGui::SetNextWindowPos({ viewport->Pos.x, viewport->Pos.y + menuBarHeight });
    ImGui::SetNextWindowSize({ viewport->Size.x, viewport->Size.y - menuBarHeight });
    ImGui::SetNextWindowViewport(viewport->ID);

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
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene")) { /* TODO */ }
            if (ImGui::MenuItem("Save Scene")) { /* TODO */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit"))
                PostQuitMessage(0);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("GameObject"))
        {
            if (ImGui::MenuItem("Create Empty")) { /* TODO Day4 */ }
            if (ImGui::MenuItem("Create Cube")) { /* TODO Day4 */ }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("CS Engine v0.1");
            ImGui::EndMenu();
        }

        // 오른쪽에 FPS 표시
        char fps[32];
        snprintf(fps, sizeof(fps), "%.1f FPS", ImGui::GetIO().Framerate);
        float fpsWidth = ImGui::CalcTextSize(fps).x + 16.f;
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - fpsWidth + ImGui::GetCursorPosX());
        ImGui::TextDisabled("%s", fps);

        ImGui::EndMainMenuBar();
    }
}

// ── Hierarchy 패널 ────────────────────────────────────────────────────
void EditorUI::DrawHierarchy(GameScene* scene)
{
    ImGui::Begin("Hierarchy");

    if (!scene)
    {
        ImGui::TextDisabled("No scene loaded");
        ImGui::End();
        return;
    }

    const auto& objects = scene->GetGameObjects();
    for (int i = 0; i < (int)objects.size(); i++)
    {
        GameObject* obj = objects[i].get();
        if (!obj) continue;

        // 선택 하이라이트
        bool isSelected = (m_selected == obj);

        // 비활성 오브젝트는 흐리게
        if (!obj->GetActive())
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.f));

        std::string label = obj->GetName();
        if (label.empty()) label = "GameObject_" + std::to_string(i);

        if (ImGui::Selectable(label.c_str(), isSelected,
            ImGuiSelectableFlags_AllowDoubleClick))
        {
            m_selected = obj;
        }

        // 우클릭 컨텍스트 메뉴
        if (ImGui::BeginPopupContextItem())
        {
            m_selected = obj;
            if (ImGui::MenuItem("Delete")) obj->Destroy();
            if (ImGui::MenuItem("Duplicate")) { /* TODO */ }
            if (ImGui::MenuItem(obj->GetActive() ? "Deactivate" : "Activate"))
                obj->SetActive(!obj->GetActive());
            ImGui::EndPopup();
        }

        if (!obj->GetActive())
            ImGui::PopStyleColor();
    }

    // 빈 공간 클릭 시 선택 해제
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) &&
        !ImGui::IsAnyItemHovered())
        m_selected = nullptr;

    ImGui::End();
}

// ── Inspector 패널 ────────────────────────────────────────────────────
void EditorUI::DrawInspector()
{
    ImGui::Begin("Inspector");

    if (!m_selected)
    {
        ImGui::TextDisabled("No object selected");
        ImGui::End();
        return;
    }

    // ── 오브젝트 이름 + 활성화 체크박스 ──────────────────────────────
    bool active = m_selected->GetActive();
    if (ImGui::Checkbox("##active", &active))
        m_selected->SetActive(active);
    ImGui::SameLine();

    std::string name = m_selected->GetName();

    ImGui::SetNextItemWidth(-1.f);
    // Enter를 누르거나 포커스를 잃었을 때만 true를 반환하도록 설정 가능
    if (ImGui::InputText("##name", &name, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        m_selected->SetName(name);
    }
    ImGui::Separator();

    // ── Transform ─────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        Transform* tr = m_selected->GetTransform();
        if (tr)
        {
            Vector3 pos = tr->GetPosition();
            Vector3 rot = tr->GetRotation();
            Vector3 scl = tr->GetScale();

            ImGui::Text("Position");
            ImGui::SetNextItemWidth(-1.f);
            if (ImGui::DragFloat3("##pos", &pos.x, 0.1f))
                tr->SetPosition(pos);

            ImGui::Text("Rotation");
            ImGui::SetNextItemWidth(-1.f);
            if (ImGui::DragFloat3("##rot", &rot.x, 1.f))
                tr->SetRotation(rot);

            ImGui::Text("Scale");
            ImGui::SetNextItemWidth(-1.f);
            if (ImGui::DragFloat3("##scl", &scl.x, 0.1f, 0.001f, 1000.f))
                tr->SetScale(scl);
        }
    }

    // ── Camera (있을 경우) ────────────────────────────────────────────
    Camera* cam = m_selected->GetComponent<Camera>();
    if (cam)
    {
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("FOV", &cam->m_fov, 0.5f, 10.f, 170.f);
            ImGui::DragFloat("Near", &cam->m_nearZ, 0.01f, 0.01f, 10.f);
            ImGui::DragFloat("Far", &cam->m_farZ, 1.f, 10.f, 10000.f);
            ImGui::Separator();
            ImGui::DragFloat("Move Speed", &cam->m_moveSpeed, 0.5f, 0.1f, 100.f);
            ImGui::DragFloat("Rot Speed", &cam->m_rotateSpeed, 0.5f, 1.f, 180.f);
        }
    }

    // ── MeshRenderer (있을 경우) ──────────────────────────────────────
    MeshRenderer* mr = m_selected->GetComponent<MeshRenderer>();
    if (mr)
    {
        if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            float col[4] = {
                mr->GetColor().x, mr->GetColor().y,
                mr->GetColor().z, mr->GetColor().w
            };
            if (ImGui::ColorEdit4("Color", col))
                mr->SetColor(col[0], col[1], col[2], col[3]);
        }
    }

    ImGui::End();
}