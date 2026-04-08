#pragma once
#include "StateMachine.h"
#include "IRender.h"

class IRenderer;
class Scene;
class SceneMachine : public StateMachine<Scene>, public IRender
{
public:
	virtual void Render(IRenderer& renderer) const override;

private:
	// 게임 오브젝트 등등
};