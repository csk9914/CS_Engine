#pragma once
#include "IGame.h"

class GameObject;
class SHOOTER : public IGame
{
public:
	SHOOTER();
	virtual ~SHOOTER() {}

public:
	virtual void Start() override;
	virtual void Update(float deltaTime) override;
	virtual void End() override;

public:
	virtual int GetWidth() const override { return m_appConfig.width; }
	virtual int GetHeight() const override { return m_appConfig.height; }

	virtual AppConfig GetAppConfig() const override { return m_appConfig; }

private:
	AppConfig m_appConfig;;

	// 테스트용 게임 오브젝트
	GameObject* m_player;

};