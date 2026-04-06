#pragma once
#include <memory>
#include <vector>
#include <Windows.h>
#include <algorithm>

class EditorUI;
class SpriteRenderer;
class MeshRenderer;
class DX11Renderer;

class RenderManager
{
public:
	RenderManager(HWND hWnd, int width, int height);

public:
	void RegisterSprite(SpriteRenderer* sprite);
	void UnregisterSprite(SpriteRenderer* sprite);

	void RegisterMesh(MeshRenderer* mesh);
	void UnregisterMesh(MeshRenderer* mesh);

	void RenderAll();

	void Release();
public:
	DX11Renderer* GetRenderer() const { return m_renderer.get(); }
	EditorUI* GetEditorUI() const { return m_editorUI.get(); }

private:
	std::unique_ptr<DX11Renderer> m_renderer;
	std::vector<SpriteRenderer*> m_sprites;
	std::vector<MeshRenderer*> m_meshes;

	std::unique_ptr<EditorUI> m_editorUI;
};