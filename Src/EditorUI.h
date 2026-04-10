#pragma once
#define NOMINMAX
#include <Windows.h>
#include <vector>
#include <string>
#include <d3d11.h>
#include <memory>
#include "imgui/imgui.h"
#include "EditorWindow.h"


class GameApp;
class Scene;
class GameObject;
class Gizmo;

class EditorUI
{
public:
	EditorUI();
	~EditorUI();

	bool Init(HWND hWnd, DX11Renderer* renderer);
	void Update(float dt, class RenderPipeline* pipeline);

	void BeginFrame();
	void Draw(GameApp* game, ID3D11ShaderResourceView* sceneSRV, ID3D11ShaderResourceView* gameSRV);
	void EndFrame();
	void Shutdown();

	// 선택 오브젝트
	GameObject* GetSelected() const { return m_selected; }
	void SetSelected(GameObject* obj) { m_selected = obj; }

	// Gizmo
	Gizmo* GetGizmo() const { return m_gizmo.get(); }

	// SceneView 위치/크기 (Ray cast & Gizmo 에임용)
	ImVec2 GetSceneViewPos()  const { return m_sceneViewPos; }
	ImVec2 GetSceneViewSize() const { return m_sceneViewSize; }

	void SetSceneViewPos(ImVec2 p) { m_sceneViewPos = p; }
	void SetSceneViewSize(ImVec2 s) { m_sceneViewSize = s; }

	void RenderTopToolbar(GameApp* game);

	void DrawAddComponentButton();

public:
	static void EditorUI::LabeledDragFloat(const char* label, float* value, float speed = 0.05f);
	static void EditorUI::LabeledDragFloat3(const char* label, float* value, float speed = 0.05f);

private:
	void DrawMainDockSpace();
	void DrawHierarchy(Scene* scene);
	void DrawInspector();
	void DrawMenuBar();

	std::unique_ptr<Gizmo> m_gizmo;
	GameObject* m_selected = nullptr;

	bool m_initialized = false;
	bool m_dockingEnabled = false;

	ImVec2 m_sceneViewPos = { 0, 0 };
	ImVec2 m_sceneViewSize = { 0, 0 };

	std::vector<std::unique_ptr<EditorWindow>> m_windows;
};
