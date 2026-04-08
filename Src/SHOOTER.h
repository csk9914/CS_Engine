#pragma once
#include "GameApp.h"
#include "Scene.h"
#include <memory>

class GameObject;
class SHOOTER : public GameApp
{
public:
	SHOOTER();
	virtual ~SHOOTER() {}

	virtual void Init() override;
};