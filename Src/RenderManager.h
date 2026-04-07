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
class RenderPipeline;

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

	void RenderAll(float deltaTime);
	void Release();

public:
	DX11Renderer* GetRenderer() const { return m_renderer.get(); }
	EditorUI* GetEditorUI() const { return m_editorUI.get(); }

private:
	// 뷰포트 좌클릭 → 오브젝트 Ray cast 선택
	void Raycast();

private:
	std::unique_ptr<DX11Renderer> m_renderer;
	std::vector<SpriteRenderer*> m_sprites;

	std::unique_ptr<RenderPipeline> m_pipeline; // 공장 추가
	std::vector<MeshRenderer*> m_meshes;		// 리스트 관리만 수행

	std::unique_ptr<EditorUI> m_editorUI;

};