#pragma once
#define NOMINMAX
#include <Windows.h>
#include <memory>
#include <vector>
#include <algorithm>

class EditorUI;
class SpriteRenderer;
class MeshRenderer;
class DX11Renderer;
class Gizmo;

class RenderManager
{
public:
	RenderManager(HWND hWnd, int width, int height);
	~RenderManager();

public:
	void RegisterSprite(SpriteRenderer* sprite);
	void UnregisterSprite(SpriteRenderer* sprite);

	void RegisterMesh(MeshRenderer* mesh);
	void UnregisterMesh(MeshRenderer* mesh);

	void Update();
	void RenderAll();
	void Release();

public:
	DX11Renderer* GetRenderer() const { return m_renderer.get(); }
	EditorUI* GetEditorUI() const { return m_editorUI.get(); }

private:
	// 뷰포트 좌클릭 → 오브젝트 Ray cast 선택
	void TryPickObject();

private:
	std::unique_ptr<DX11Renderer> m_renderer;
	std::vector<SpriteRenderer*> m_sprites;
	std::vector<MeshRenderer*> m_meshes;

	std::unique_ptr<EditorUI> m_editorUI;
	std::unique_ptr<Gizmo>        m_gizmo;
};