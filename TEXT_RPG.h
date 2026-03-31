#pragma once
#include "IGame.h"

class IRenderer;
class TEXT_RPG : public IGame
{
public:
	TEXT_RPG() {}
	virtual ~TEXT_RPG(){}

public:
	virtual void Start() override;
	virtual void Update(float deltaTime) override;
	virtual void Render(IRenderer& renderer) const override;
	virtual void End() override;
};