#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"

class DX11Renderer;
class GameObject;
class Camera;

class Gizmo
{
public:
	Gizmo();
	~Gizmo();

	bool Init();

	void BeginFrame();
	void Update();
	void Render();

	void SetTarget(GameObject* obj) { m_target = obj; }
	GameObject* GetTartget() const { return m_target; }

	bool IsCtrl() const { return ImGuizmo::IsUsing(); }

private:
	GameObject* m_target = nullptr;

	// 기즈모 설정 상태
	ImGuizmo::OPERATION m_currentOp = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE m_currentMode = ImGuizmo::WORLD;

	bool m_initialized = false;
};