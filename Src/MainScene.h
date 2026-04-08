#pragma once
#include "Scene.h"

class MainScene : public Scene
{
public:
	virtual void OnEnter() override;
	virtual void Update(float deltaTime) override;
	virtual void OnExit() override;

private:
	GameObject* m_moveObj;
};