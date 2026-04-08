#include "SceneMachine.h"
#include "Scene.h"
#include "IRenderer.h"
#include "GameObject.h"

void SceneMachine::Render(IRenderer& renderer) const
{
	if (GetCurState())
	{
		//GetCurState()->Render();
	}
}
