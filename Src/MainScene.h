#pragma once
#include "Scene.h"

class MainScene : public Scene
{
public:
	virtual void OnEnter() override;
	virtual void OnExit() override;
};