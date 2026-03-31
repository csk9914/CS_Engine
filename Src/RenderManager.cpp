#include "RenderManager.h"
#include "SpriteRenderer.h"

#include "Renderer.h"

RenderManager::RenderManager(int width, int height, bool visible)
{
	m_renderer = Renderer::CreateRenderer(width, height, visible);

	if (m_renderer == nullptr)
	{
		return;
	}
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

void RenderManager::RenderAll()
{
	if (!m_renderer) return;

	// 순서대로 정렬(레이어 낮은 순서부터)

	// 모든 스프라이트 그리기
	for (const auto& sprite : m_sprites)
	{
		if (sprite)
			sprite->Render();
	}

	// 버퍼 전환
	m_renderer->Flip();
}
