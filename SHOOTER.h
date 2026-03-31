#pragma once
#include "IGame.h"

class IRenderer;
class SHOOTER : public IGame
{
public:
	SHOOTER() {}
	virtual ~SHOOTER() {}

public:
	virtual void Start() override;
	virtual void Update(float deltaTime) override;
	virtual void Render(IRenderer& renderer) const override;
	virtual void End() override;
};