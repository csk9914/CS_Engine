#pragma once
#include "IGame.h"

class TEXT_RPG : public IGame
{
public:
	TEXT_RPG() :m_config() {}

	virtual ~TEXT_RPG() {}

public:
	virtual void Start() override;
	virtual void Update(float deltaTime) override;
	virtual void End() override;

public:
	virtual int GetWidth() const override { return m_config.width; }
	virtual int GetHeight() const override { return m_config.height; }
	virtual AppConfig GetAppConfig() const override { return m_config; }

private:
	AppConfig m_config;;


};