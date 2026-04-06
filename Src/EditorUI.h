#pragma once

#include <vector>
#include <string>
#include <Windows.h>
#include <d3d11.h>

class GameScene;
class GameObject;

class EditorUI
{
public:
	EditorUI() = default;
	~EditorUI();

	bool Init(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context);

	void BeginFrame();
	void Draw(GameScene* scene);
	void EndFrame();

	void Shutdown();

	// 현재 선택된 오브젝트 
	GameObject* GetSelected() const { return m_selected; }
	void SetSelected(GameObject* obj) { m_selected = obj; }


private:
	void DrawMainDockSpace();
	void DrawHierarchy(GameScene* scene);
	void DrawInspector();
	void DrawMenuBar();

private:
	bool m_initialized = false;
	bool m_dockingEnabled = false;

	GameObject* m_selected = nullptr;
};