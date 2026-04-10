#pragma once
#pragma once
#include "EditorWindow.h"
#include <filesystem>
#include <string>
#include <vector>
#include <functional>

#include <imgui.h>

namespace fs = std::filesystem;

// 파일 아이템 하나를 나타냄
struct AssetItem
{
    std::string name;       // 표시 이름
    fs::path    fullPath;   // 전체 경로
    bool        isDir;      // 폴더 여부
};

class ProjectPanel : public EditorWindow
{
public:
    ProjectPanel(EditorUI* editorUI, DX11Renderer* renderer,
        const std::string& rootPath);
    virtual ~ProjectPanel() override = default;

    virtual void Init()   override;
    virtual void Update(float dt, RenderPipeline* pipeline) override {}
    virtual void OnGUI()  override;

private:
    // ── 왼쪽: 폴더 트리 ──────────────────────────────────────────────
    void DrawFolderTree(const fs::path& dir, int depth = 0);

    // ── 오른쪽: 파일 그리드 ──────────────────────────────────────────
    void DrawFileGrid();
    void DrawAssetItem(const AssetItem& item, int index);

    // ── 컨텍스트 메뉴 ────────────────────────────────────────────────
    void DrawFolderContextMenu(const fs::path& dir);
    void DrawFileContextMenu(const AssetItem& item);
    void DrawEmptyAreaContextMenu();

    // ── 유틸 ─────────────────────────────────────────────────────────
    void            RefreshCurrentDir();
    void            NavigateTo(const fs::path& dir);
    void            OpenFile(const fs::path& path);
    void            CreateNewFolder(const fs::path& parent);
    void            CreateNewScript(const fs::path& parent, const std::string& name);
    void            DeleteAsset(const fs::path& path);

    // 확장자별 색상 / 레이블
    const char* GetFileIcon(const fs::path& path) const;
    ImVec4          GetFileColor(const fs::path& path) const;

    // 이름 변경 상태
    void            BeginRename(const fs::path& target);
    void            CommitRename();

private:
    fs::path                m_rootPath;     // Assets 루트
    fs::path                m_curDir;       // 현재 보고 있는 폴더
    std::vector<AssetItem>  m_items;        // 현재 폴더의 파일 목록 (캐시)
    bool                    m_dirty = true; // 목록 새로고침 필요 여부

    // 선택 상태
    int                     m_selectedIdx = -1;
    fs::path                m_selectedPath;

    // 이름 변경 상태
    bool                    m_renaming = false;
    fs::path                m_renamePath;
    char                    m_renameBuf[256] = {};

    // 썸네일 크기 (슬라이더로 조절)
    float                   m_iconSize = 72.f;

    // 폴더 트리에서 선택된 폴더 (하이라이트용)
    fs::path                m_treeSelected;

private:
    void RenderCreateScriptModal(); // 팝업 렌더링 함수

    // 팝업 관련 상태 변수들
    bool m_showCreateScriptModal = false;
    char m_newScriptNameBuf[128] = "NewBehaviour";
    fs::path m_createTargetDir;

};