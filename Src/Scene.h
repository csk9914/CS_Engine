#pragma once
#include "IState.h"
#include "IRender.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <string>

#include "CSQueue.h"

class IRenderer;
class Component;
class GameObject;
class Rigidbody;

class Scene : public IState
{
public:
	virtual ~Scene();
	virtual void Update(float deltaTime) override;

	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual std::string GetStateID() const override { return "GameScene"; }

	void ProcessCompAwake();
	void ProcessAwake();
	void ProcessStart();
	void ProcessFixedUpdate(float deltaTime);
	void ProcessUpdate(float deltaTime);
	void ProcessDestroy();

	// 씬에 게임오브젝트 추가
	GameObject* CreateGameObject(const std::string& name = "GameObject");

	// 오브젝트 접근
	const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const { return m_gameObjects; }

	void RegisterForRigidbody(Rigidbody* rigid);
	void UnRegisterForRigidbody(Rigidbody* rigid);


	bool IsPlaying() const;

private:
	std::vector<std::unique_ptr<GameObject>> m_gameObjects;
	std::vector<Rigidbody*> m_rigids;

	CSQueue<Component*> m_compAwakeQueue;
	CSQueue<GameObject*> m_awakeQueue;
	CSQueue<GameObject*> m_startQueue;


};