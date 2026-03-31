#pragma once
#include "IState.h"
#include "IRender.h"
#include <memory>
#include <vector>
class IRenderer;
class GameObject;
class GameScene : public IState, public IRender
{
public:
	virtual ~GameScene() {}

	virtual void Update(float deltaTime) override;
	virtual void Render(IRenderer& renderer) const override;
	virtual void OnEnter() override;
	virtual void OnExit() override;

private:
	std::vector<std::unique_ptr<GameObject>> m_gameObjects;

	//virtual std::string GetStateID() const override;
};