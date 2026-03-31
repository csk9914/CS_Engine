#include "SpriteRenderer.h"
#include "GameEngine.h"
#include "RenderManager.h"
#include "GameObject.h"
#include "Renderer.h"

SpriteRenderer::~SpriteRenderer()
{
	// 혹시라도 OnDisable/OnDestroy가 호출되지 않고 메모리가 해제될 경우를 대비
	GameEngine::Instance()->GetRenderManager()->UnregisterSprite(this);
}

void SpriteRenderer::Render()
{
	Transform* transform = GetOwner()->GetTransform();
	if (!transform) return;


	// 콘솔이라 int 캐스팅
	int x = static_cast<int>(transform->GetPosition().x);
	int y = static_cast<int>(transform->GetPosition().y);

	RenderManager* renderManager = GameEngine::Instance()->GetRenderManager();
	if (renderManager)
	{
		renderManager->GetRenderer()->RenderText(x, y, m_sprite);
	}

}

void SpriteRenderer::OnEnable()
{
	GameEngine::Instance()->GetRenderManager()->RegisterSprite(this);
}

void SpriteRenderer::OnDisable()
{
	GameEngine::Instance()->GetRenderManager()->UnregisterSprite(this);
}

void SpriteRenderer::OnDestroy()
{
	GameEngine::Instance()->GetRenderManager()->UnregisterSprite(this);
}
