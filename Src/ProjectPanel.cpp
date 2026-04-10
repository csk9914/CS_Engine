#include "ProjectPanel.h"
#include "EditorUI.h"
#include "imgui.h"
#include <algorithm>
#include <fstream>
#include <shellapi.h>   // ShellExecuteW
#include <iostream>

#pragma comment(lib, "Shell32.lib")

namespace fs = std::filesystem;

// 생성자
ProjectPanel::ProjectPanel(EditorUI* editorUI, DX11Renderer* renderer,
	const std::string& rootPath)
	: EditorWindow("Project", editorUI, renderer)
	, m_rootPath(rootPath)
	, m_curDir(rootPath)
	, m_treeSelected(rootPath)
{
}

void ProjectPanel::Init()
{
	// Assets 루트 폴더가 없으면 생성
	if (!fs::exists(m_rootPath))
		fs::create_directories(m_rootPath);

	// 기본 하위 폴더 생성
	fs::create_directories(m_rootPath / "Scripts");
	fs::create_directories(m_rootPath / "Scenes");
	fs::create_directories(m_rootPath / "Textures");
	fs::create_directories(m_rootPath / "Materials");
	fs::create_directories(m_rootPath / "Meshes");

	RefreshCurrentDir();
}

// 파일 목록 새로고침 
void ProjectPanel::RefreshCurrentDir()
{
	m_items.clear();
	if (!fs::exists(m_curDir)) return;

	// 폴더 먼저, 그 다음 파일 (이름순 정렬)
	std::vector<AssetItem> dirs, files;

	for (auto& entry : fs::directory_iterator(m_curDir))
	{
		AssetItem item;
		item.fullPath = entry.path();
		item.name = entry.path().filename().string();
		item.isDir = entry.is_directory();

		if (item.isDir) dirs.push_back(item);
		else            files.push_back(item);
	}

	auto byName = [](const AssetItem& a, const AssetItem& b)
		{
			return a.name < b.name;
		};
	std::sort(dirs.begin(), dirs.end(), byName);
	std::sort(files.begin(), files.end(), byName);

	m_items.insert(m_items.end(), dirs.begin(), dirs.end());
	m_items.insert(m_items.end(), files.begin(), files.end());
	m_dirty = false;
}

void ProjectPanel::NavigateTo(const fs::path& dir)
{
	m_curDir = dir;
	m_treeSelected = dir;
	m_selectedIdx = -1;
	m_dirty = true;
}

// OnGUI
void ProjectPanel::OnGUI()
{
	if (m_dirty) RefreshCurrentDir();

	ImGui::Begin("Project");

	// 상단 툴바 (경로 표시 + 아이콘 크기 슬라이더) 
	{
		// 현재 경로를 루트 기준 상대 경로로 표시
		std::string rel = fs::relative(m_curDir, m_rootPath).string();
		if (rel == ".") rel = "Assets";
		else            rel = "Assets/" + rel;

		ImGui::TextDisabled("%s", rel.c_str());
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 130.f);
		ImGui::SetNextItemWidth(130.f);
		ImGui::SliderFloat("##sz", &m_iconSize, 48.f, 128.f, "%.0f");
		ImGui::Separator();
	}

	// 2단 레이아웃
	float treeWidth = 180.f;
	ImGui::BeginChild("##tree", ImVec2(treeWidth, 0), true);
	DrawFolderTree(m_rootPath);
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("##grid", ImVec2(0, 0), false);
	DrawFileGrid();
	ImGui::EndChild();

	// 마지막에 팝업 함수 호출
	RenderCreateScriptModal();

	ImGui::End();
}

// 왼쪽 폴더 트리
void ProjectPanel::DrawFolderTree(const fs::path& dir, int depth)
{
	std::string name = (depth == 0) ? "Assets" : dir.filename().string();
	std::string id = "##tree_" + dir.string();

	ImGuiTreeNodeFlags flags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_SpanAvailWidth;

	if (m_treeSelected == dir)
		flags |= ImGuiTreeNodeFlags_Selected;

	// 루트는 기본으로 열려있게
	if (depth == 0) flags |= ImGuiTreeNodeFlags_DefaultOpen;

	bool open = ImGui::TreeNodeEx((name + id).c_str(), flags, "%s", name.c_str());

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		NavigateTo(dir);

	// 폴더 우클릭 메뉴
	if (ImGui::BeginPopupContextItem(("treeCtx_" + dir.string()).c_str()))
	{
		DrawFolderContextMenu(dir);
		ImGui::EndPopup();
	}

	if (open)
	{
		// 하위 폴더만 재귀
		for (auto& entry : fs::directory_iterator(dir))
		{
			if (entry.is_directory())
				DrawFolderTree(entry.path(), depth + 1);
		}
		ImGui::TreePop();
	}
}

// 오른쪽 파일 그리드
void ProjectPanel::DrawFileGrid()
{
	// 빈 영역 우클릭
	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)
		&& !ImGui::IsAnyItemHovered())
	{
		ImGui::OpenPopup("##emptyCtx");
	}
	if (ImGui::BeginPopup("##emptyCtx"))
	{
		DrawEmptyAreaContextMenu();
		ImGui::EndPopup();
	}

	// 아이콘 그리드
	float cellSize = m_iconSize + 16.f;
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int   columns = std::max(1, (int)(panelWidth / cellSize));

	ImGui::Columns(columns, nullptr, false);

	for (int i = 0; i < (int)m_items.size(); i++)
	{
		DrawAssetItem(m_items[i], i);
		ImGui::NextColumn();
	}

	ImGui::Columns(1);
}

// 아이템 하나 그리기 
void ProjectPanel::DrawAssetItem(const AssetItem& item, int index)
{
	bool selected = (m_selectedIdx == index);
	ImVec4 color = GetFileColor(item.fullPath);
	const char* icon = GetFileIcon(item.fullPath);

	// 이름 변경 모드
	bool isRenaming = (m_renaming && m_renamePath == item.fullPath);

	ImGui::PushID(index);

	// 아이콘 버튼
	ImGui::PushStyleColor(ImGuiCol_Button, selected
		? ImVec4(0.26f, 0.59f, 0.98f, 0.5f)
		: ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));

	// 아이콘 텍스트를 버튼처럼 (실제 텍스처가 없으므로 컬러 텍스트로 대체)
	ImGui::BeginGroup();

	ImVec2 iconPos = ImGui::GetCursorScreenPos();
	ImGui::Dummy(ImVec2(m_iconSize, m_iconSize));
	ImDrawList* dl = ImGui::GetWindowDrawList();

	// 선택/호버 배경
	if (selected)
		dl->AddRectFilled(iconPos,
			ImVec2(iconPos.x + m_iconSize, iconPos.y + m_iconSize),
			IM_COL32(66, 150, 250, 80), 4.f);

	// 아이콘 텍스트 (중앙 정렬)
	float fontSize = m_iconSize * 0.45f;
	fontSize = std::clamp(fontSize, 14.f, 40.f);
	ImVec2 iconCenter = ImVec2(
		iconPos.x + m_iconSize * 0.5f,
		iconPos.y + m_iconSize * 0.4f);
	dl->AddText(nullptr, fontSize,
		ImVec2(iconCenter.x - fontSize * 0.5f, iconCenter.y - fontSize * 0.5f),
		ImGui::ColorConvertFloat4ToU32(color),
		icon);

	ImGui::PopStyleColor(2);

	// 파일명 (이름 변경 모드)
	if (isRenaming)
	{
		ImGui::SetNextItemWidth(m_iconSize);

		if (ImGui::IsWindowAppearing() || m_renaming) ImGui::SetKeyboardFocusHere();

		if (ImGui::InputText("##rename", m_renameBuf, sizeof(m_renameBuf),
			ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			CommitRename();
		}

		if (ImGui::IsItemDeactivated() && !ImGui::IsItemActive())
			m_renaming = false;

	}
	else
	{
		// 이름 텍스트 (잘림 처리)
		ImGui::SetNextItemWidth(m_iconSize);
		std::string display = item.name;
		if (ImGui::CalcTextSize(display.c_str()).x > m_iconSize)
		{
			while (display.size() > 3 &&
				ImGui::CalcTextSize((display + "...").c_str()).x > m_iconSize)
				display.pop_back();
			display += "...";
		}
		ImGui::TextUnformatted(display.c_str());
	}

	ImGui::EndGroup();

	// 클릭 감지 (더미 위에)
	ImGui::SetCursorScreenPos(iconPos);
	ImGui::InvisibleButton(("##btn" + item.name).c_str(),
		ImVec2(m_iconSize, m_iconSize + 20.f));

	// 단일 클릭 → 선택
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		m_selectedIdx = index;
		m_selectedPath = item.fullPath;
		m_renaming = false;
	}

	// 더블클릭
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		if (item.isDir)
			NavigateTo(item.fullPath);
		else
			OpenFile(item.fullPath);
	}

	// 우클릭 컨텍스트 메뉴
	if (ImGui::BeginPopupContextItem(("##ctx" + item.name).c_str()))
	{
		m_selectedIdx = index;
		m_selectedPath = item.fullPath;
		if (item.isDir) DrawFolderContextMenu(item.fullPath);
		else            DrawFileContextMenu(item);
		ImGui::EndPopup();
	}

	// 드래그 앤 드롭 소스 (나중에 Inspector에 드롭용)
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		std::string pathStr = item.fullPath.string();
		ImGui::SetDragDropPayload("ASSET_PATH", pathStr.c_str(), pathStr.size() + 1);
		ImGui::Text("%s %s", icon, item.name.c_str());
		ImGui::EndDragDropSource();
	}

	ImGui::PopID();
}

// 컨텍스트 메뉴들 
void ProjectPanel::DrawFolderContextMenu(const fs::path& dir)
{
	if (ImGui::BeginMenu("Create"))
	{
		if (ImGui::MenuItem("Folder"))
			CreateNewFolder(dir);
		ImGui::Separator();
		if (ImGui::MenuItem("C++ Script"))
		{
			m_showCreateScriptModal = true;
			m_createTargetDir = dir;
			strcpy_s(m_newScriptNameBuf, "NewScript");


			//CreateNewScript(dir, "NewScript");
			//m_dirty = true;
		}
		ImGui::EndMenu();
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Show in Explorer"))
	{
		std::wstring ws = dir.wstring();
		ShellExecuteW(nullptr, L"explore", ws.c_str(), nullptr, nullptr, SW_SHOW);
	}
	// 루트는 삭제 불가
	if (dir != m_rootPath)
	{
		ImGui::Separator();
		if (ImGui::MenuItem("Delete"))
		{
			DeleteAsset(dir);
			if (m_curDir == dir) NavigateTo(m_rootPath);
		}
		if (ImGui::MenuItem("Rename"))
			BeginRename(dir);
	}
}

void ProjectPanel::DrawFileContextMenu(const AssetItem& item)
{
	if (ImGui::MenuItem("Open"))
		OpenFile(item.fullPath);
	if (ImGui::MenuItem("Show in Explorer"))
	{
		std::wstring ws = item.fullPath.parent_path().wstring();
		ShellExecuteW(nullptr, L"explore", ws.c_str(), nullptr, nullptr, SW_SHOW);
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Rename"))
		BeginRename(item.fullPath);
	if (ImGui::MenuItem("Delete"))
		DeleteAsset(item.fullPath);
}

void ProjectPanel::DrawEmptyAreaContextMenu()
{
	if (ImGui::BeginMenu("Create"))
	{
		if (ImGui::MenuItem("Folder"))
			CreateNewFolder(m_curDir);
		ImGui::Separator();
		if (ImGui::MenuItem("C++ Script"))
		{
			m_showCreateScriptModal = true;
			m_createTargetDir = m_curDir;
			strcpy_s(m_newScriptNameBuf, "NewScript");


			//CreateNewScript(m_curDir, "NewScript");
			//m_dirty = true;
		}
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Show in Explorer"))
	{
		std::wstring ws = m_curDir.wstring();
		ShellExecuteW(nullptr, L"explore", ws.c_str(), nullptr, nullptr, SW_SHOW);
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Refresh"))
		m_dirty = true;
}

// 파일 작업
void ProjectPanel::OpenFile(const fs::path& path)
{
	std::wstring ws = path.wstring();
	ShellExecuteW(nullptr, L"open", ws.c_str(), nullptr, nullptr, SW_SHOW);
}

void ProjectPanel::CreateNewFolder(const fs::path& parent)
{
	// "New Folder", "New Folder (1)" ... 중복 방지
	fs::path newDir = parent / "New Folder";
	int n = 1;
	while (fs::exists(newDir))
		newDir = parent / ("New Folder (" + std::to_string(n++) + ")");

	fs::create_directory(newDir);
	m_dirty = true;
	BeginRename(newDir);
}

void ProjectPanel::CreateNewScript(const fs::path& parent, const std::string& name)
{
	// 중복 방지
	fs::path hPath = parent / (name + ".h");
	fs::path cppPath = parent / (name + ".cpp");
	int n = 1;
	while (fs::exists(hPath))
	{
		std::string newName = name + std::to_string(n++);
		hPath = parent / (newName + ".h");
		cppPath = parent / (newName + ".cpp");
	}

	std::string className = hPath.stem().string();

	// .h 생성
	{
		std::ofstream f(hPath);
		f << "#pragma once\n";
		f << "#include \"MonoBehaviour.h\"\n\n";
		f << "class " << className << " : public MonoBehaviour\n";
		f << "{\n";
		f << "public:\n";
		f << "\t" << className << "(std::string name = \"" << className << "\") : MonoBehaviour(name) {}\n";
		f << "\tvirtual void Start()              override;\n";
		f << "\tvirtual void Update(float dt)     override;\n";
		f << "};\n";
	}

	// .cpp 생성
	{
		std::ofstream f(cppPath);
		f << "#include \"" << className << ".h\"\n\n";
		f << "#include \"ComponentRegistry.h\"\n\n";
		f << "#include \"GameObject.h\"\n\n";

		f << "void " << className << "::Start()\n{\n}\n\n";
		f << "void " << className << "::Update(float dt)\n{\n}\n";

		f << "\nREGISTER_COMPONENT(" << className << ", \"Scripts\")";
	}

	// 1. 현재 작업 디렉터리(CS_Engine/Src) 획득
	fs::path currentPath = fs::current_path();

	// 2. 한 단계 위인 프로젝트 루트(CS_Engine) 획득
	fs::path projectRoot = currentPath.parent_path();

	// 3. setup.bat 위치 (루트 바로 아래에 있다고 하셨으니)
	fs::path setupBatPath = projectRoot / "setup.bat";

	if (fs::exists(setupBatPath))
	{
		// cd /d로 루트 폴더로 이동한 뒤 bat 실행
		std::string cmd = "cmd.exe /c cd /d \"" + projectRoot.string() + "\" && cmake -S . -B build";

		WinExec(cmd.c_str(), SW_HIDE);
		std::cout << "루트 폴더에서 setup.bat 실행!: " << projectRoot.string() << std::endl;
	}
	else
	{
		// 만약 못 찾으면 어디를 찔렀는지 출력 (디버깅용)
		std::cout << "setup.bat 못 찾음. 시도 경로: " << setupBatPath.string() << std::endl;
	}


	m_dirty = true;

}

void ProjectPanel::DeleteAsset(const fs::path& path)
{
	std::error_code ec;
	fs::remove_all(path, ec);
	if (m_selectedPath == path)
	{
		m_selectedIdx = -1;
		m_selectedPath = "";
	}

	// 1. 현재 작업 디렉터리(CS_Engine/Src) 획득
	fs::path currentPath = fs::current_path();

	// 2. 한 단계 위인 프로젝트 루트(CS_Engine) 획득
	fs::path projectRoot = currentPath.parent_path();

	// 3. setup.bat 위치 (루트 바로 아래에 있다고 하셨으니)
	fs::path setupBatPath = projectRoot / "setup.bat";

	if (fs::exists(setupBatPath))
	{
		// cd /d로 루트 폴더로 이동한 뒤 bat 실행
		std::string cmd = "cmd.exe /c cd /d \"" + projectRoot.string() + "\" && cmake -S . -B build";

		WinExec(cmd.c_str(), SW_HIDE);
		std::cout << "루트 폴더에서 setup.bat 실행!: " << projectRoot.string() << std::endl;
	}
	else
	{
		// 만약 못 찾으면 어디를 찔렀는지 출력 (디버깅용)
		std::cout << "setup.bat 못 찾음. 시도 경로: " << setupBatPath.string() << std::endl;
	}

	m_dirty = true;
}

void ProjectPanel::BeginRename(const fs::path& target)
{
	m_renaming = true;
	m_renamePath = target;
	std::string name = target.filename().string();
	strncpy_s(m_renameBuf, name.c_str(), sizeof(m_renameBuf) - 1);
	m_dirty = true;
}

void ProjectPanel::CommitRename()
{
	if (!m_renaming) return;
	std::string newName(m_renameBuf);
	if (newName.empty() || newName == m_renamePath.filename().string())
	{
		m_renaming = false;
		return;
	}
	fs::path newPath = m_renamePath.parent_path() / newName;
	std::error_code ec;
	fs::rename(m_renamePath, newPath, ec);
	m_renaming = false;
	m_dirty = true;
}

void ProjectPanel::RenderCreateScriptModal()
{
	// 1. 외부에서 플래그를 켰을 때 팝업 열기 요청
	if (m_showCreateScriptModal)
	{
		ImGui::OpenPopup("Create C++ Script");
		m_showCreateScriptModal = false;
	}

	// 2. 실제 모달 창 그리기
	// AlwaysAutoResize를 주면 입력창 크기에 딱 맞게 조절됩니다.
	if (ImGui::BeginPopupModal("Create C++ Script", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Enter Script Name:");
		ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 약간의 간격

		// 팝업이 처음 열릴 때 입력창에 포커스를 줌 (바로 타이핑 가능)
		if (ImGui::IsWindowAppearing())
			ImGui::SetKeyboardFocusHere();

		// 입력창
		if (ImGui::InputText("##ScriptName", m_newScriptNameBuf, sizeof(m_newScriptNameBuf),
			ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			CreateNewScript(m_createTargetDir, m_newScriptNameBuf);
			ImGui::CloseCurrentPopup();
		}

		ImGui::TextDisabled("Note: .h and .cpp files will be created.");
		ImGui::Separator();

		// 하단 버튼들
		if (ImGui::Button("Create", ImVec2(120, 0)))
		{
			CreateNewScript(m_createTargetDir, m_newScriptNameBuf);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

//아이콘 / 색상
const char* ProjectPanel::GetFileIcon(const fs::path& path) const
{
	if (fs::is_directory(path)) return "[D]";

	std::string ext = path.extension().string();
	if (ext == ".h" || ext == ".hpp") return "[H]";
	if (ext == ".cpp" || ext == ".cc")  return "[C]";
	if (ext == ".hlsl" || ext == ".fx")  return "[S]";
	if (ext == ".png" || ext == ".jpg"
		|| ext == ".bmp" || ext == ".dds") return "[I]";
	if (ext == ".fbx" || ext == ".obj"
		|| ext == ".mesh")                 return "[M]";
	if (ext == ".scene")                return "[SC]";
	if (ext == ".mat")                  return "[MT]";
	return "[F]";
}

ImVec4 ProjectPanel::GetFileColor(const fs::path& path) const
{
	if (fs::is_directory(path))
		return ImVec4(0.98f, 0.78f, 0.25f, 1.f);   // 폴더 → 노랑

	std::string ext = path.extension().string();
	if (ext == ".h" || ext == ".hpp")
		return ImVec4(0.40f, 0.80f, 1.00f, 1.f);   // 헤더 → 하늘
	if (ext == ".cpp" || ext == ".cc")
		return ImVec4(0.50f, 1.00f, 0.50f, 1.f);   // cpp → 초록
	if (ext == ".hlsl" || ext == ".fx")
		return ImVec4(1.00f, 0.60f, 0.20f, 1.f);   // 셰이더 → 주황
	if (ext == ".png" || ext == ".jpg"
		|| ext == ".bmp" || ext == ".dds")
		return ImVec4(1.00f, 0.50f, 0.80f, 1.f);   // 이미지 → 분홍
	if (ext == ".fbx" || ext == ".obj"
		|| ext == ".mesh")
		return ImVec4(0.80f, 0.60f, 1.00f, 1.f);   // 메시 → 보라
	if (ext == ".scene")
		return ImVec4(0.30f, 0.70f, 1.00f, 1.f);   // 씬 → 파랑
	if (ext == ".mat")
		return ImVec4(0.60f, 1.00f, 0.70f, 1.f);   // 매테리얼 → 민트

	return ImVec4(0.75f, 0.75f, 0.75f, 1.f);       // 기타 → 회색
}