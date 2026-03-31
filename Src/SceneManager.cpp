#include "SceneManager.h"
#include "GameScene.h"
#include "IRenderer.h"
#include "GameObject.h"

void SceneManager::Render(IRenderer& renderer) const
{
	if (GetCurState())
	{
		GetCurState()->Render(renderer);
	}
}
