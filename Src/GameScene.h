#pragma once
#include "IState.h"
#include "IRender.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <string>

class IRenderer;
class GameObject;

class GameScene : public IState
{
public:
	virtual ~GameScene();

	virtual void Update(float deltaTime) override;
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual std::string GetStateID() const override { return "GameScene"; }

	// 씬에 게임오브젝트 추가
	GameObject* AddGameObject(const std::string& name = "GameObject");

	// 오브젝트 접근
	const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const
	{
		return m_gameObjects;
	}

private:
	std::vector<std::unique_ptr<GameObject>> m_gameObjects;


};