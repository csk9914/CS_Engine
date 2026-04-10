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
    virtual void OnExit()  override;
    virtual std::string GetStateID() const override { return "Scene"; }

    // 프레임 단계별 처리
    void ProcessAwake();
    void ProcessStart();
    void ProcessFixedUpdate(float deltaTime);   // 물리/충돌 — Play 중에만
    void ProcessUpdate(float deltaTime);
    void ProcessDestroy();

    // 오브젝트 생성 
    GameObject* CreateGameObject(const std::string& name = "GameObject");

    const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const
    {
        return m_gameObjects;
    }

    //Rigidbody 등록 
    void RegisterForRigidbody(Rigidbody* rigid);
    void UnRegisterForRigidbody(Rigidbody* rigid);

    // Play 상태 
    bool IsPlaying() const;

    // Play 시작/정지 시 Scene이 직접 처리
    void OnPlayStart();
    void OnPlayStop();

private:
    std::vector<std::unique_ptr<GameObject>> m_gameObjects;
    std::vector<Rigidbody*>                  m_fixed;

    CSQueue<Component*>  m_compAwakeQueue;
    CSQueue<GameObject*> m_awakeQueue;
    CSQueue<GameObject*> m_startQueue;
};