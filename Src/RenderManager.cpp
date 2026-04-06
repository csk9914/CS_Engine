#include "RenderManager.h"
#include "SpriteRenderer.h"

#include "DX11Renderer.h"
#include "MeshRenderer.h"
#include "EditorUI.h"
#include "GameEngine.h"

RenderManager::RenderManager(HWND hWnd, int width, int height)
{
	m_renderer = DX11Renderer::Create(hWnd, width, height);

	// 4. EditorUI (ImGui)
	m_editorUI = std::make_unique<EditorUI>();
	m_editorUI->Init(hWnd, m_renderer->GetDevice(), m_renderer->GetContext());

}

void RenderManager::RegisterSprite(SpriteRenderer* sprite)
{
	m_sprites.push_back(sprite);
}

void RenderManager::UnregisterSprite(SpriteRenderer* sprite)
{
	// 벡터에서 해당 스프라이트 제거
	// std::remove는 제거할 요소를 벡터의 끝으로 이동시키고, 실제로 제거된 요소는 벡터의 끝에 남아있게 됨
	// 따라서 erase를 사용하여 벡터의 끝에서 제거된 요소들을 삭제
	// 원본이 삭제되는 것은 아님!!
	m_sprites.erase(std::remove(m_sprites.begin(), m_sprites.end(), sprite), m_sprites.end());
}

void RenderManager::RegisterMesh(MeshRenderer* mesh)
{
	m_meshes.push_back(mesh);
}

void RenderManager::UnregisterMesh(MeshRenderer* mesh)
{
	m_meshes.erase(
		std::remove(m_meshes.begin(), m_meshes.end(), mesh),
		m_meshes.end());
}

void RenderManager::RenderAll()
{
	if (!m_renderer) return;

	// 1. 프레임 시작 (화면 클리어)
	m_renderer->BeginFrame(0.05f, 0.05f, 0.1f);

	// 2. MeshRenderer 전부 그리기 (나중에 orderInLayer 정렬 추가 가능)
	for (MeshRenderer* mesh : m_meshes)
		if (mesh) mesh->Render();

	// 모든 스프라이트 그리기
	for (const auto& sprite : m_sprites)
	{
		if (sprite) sprite->Render();
	}

	// ── ImGui (씬 위에 덮어서 그림) ─────────────────────────────
	m_editorUI->BeginFrame();
	m_editorUI->Draw(GameEngine::Instance()->GetCurrentScene()); // IGame에서 씬 가져옴
	m_editorUI->EndFrame();

	// 버퍼 전환
	m_renderer->Flip();
}

void RenderManager::Release()
{
	if (m_editorUI)
		m_editorUI->Shutdown();
}
