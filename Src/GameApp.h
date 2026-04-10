#pragma once
#include "IGameApp.h"
#include "SceneMachine.h"

class GameApp : public IGameApp
{
public:
	GameApp() = default;
	virtual ~GameApp() = default;

public:

	virtual void Awake() override {}
	virtual void Init() override {}
	virtual void Update(float deltaTime) override;
	virtual void End() override;


public:
	virtual int GetWidth() const override { return m_appConfig.width; }
	virtual int GetHeight() const override { return m_appConfig.height; }

	virtual const AppConfig& GetAppConfig() const override { return m_appConfig; }

	virtual Scene* GetCurrentScene() override { return m_sceneMachine.GetCurState(); }

public:
	void SetFirstScene(std::unique_ptr<Scene> scene);

	virtual bool IsPlay() const override { return m_isPlay; }
	void SetIsPlay(bool play);

protected:
	AppConfig m_appConfig = {};
	SceneMachine m_sceneMachine = {};
	std::unique_ptr<Scene> m_pendingScene = nullptr;

	bool m_isPlay = false;
};