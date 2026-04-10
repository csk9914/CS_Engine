#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include "IRenderer.h"
#include "MeshRenderer.h"
#include "Rigidbody.h"
#include "GameEngine.h"
#include "GameApp.h"
#include "CollisionManager.h"
#include "Camera.h"

Scene::~Scene() {}

void Scene::Update(float deltaTime)
{
    ProcessAwake();
    ProcessStart();
    ProcessFixedUpdate(deltaTime); // 내부에서 IsPlaying() 체크
    ProcessUpdate(deltaTime);
    ProcessDestroy();
}

void Scene::ProcessAwake()
{
    if (m_awakeQueue.empty()) return;

    for (auto obj : m_awakeQueue.prepare())
    {
        obj->Awake();
        m_startQueue.push(obj);
    }
}

void Scene::ProcessStart()
{
    if (m_startQueue.empty()) return;
    for (auto obj : m_startQueue.prepare())
        obj->Start();
}

void Scene::ProcessFixedUpdate(float deltaTime)
{
    // 에디터 모드에서는 물리/충돌 실행 안 함 
    if (!IsPlaying()) return;

    CollisionManager::Instance()->Update();

    for (auto* rb : m_fixed)
        rb->FixedUpdate(deltaTime);
}

void Scene::ProcessUpdate(float deltaTime)
{
    for (auto& obj : m_gameObjects)
        obj->Update(deltaTime);
}

void Scene::ProcessDestroy()
{
    m_gameObjects.erase(
        std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
            [](const std::unique_ptr<GameObject>& obj)
            {
                if (obj->IsDestroy()) { obj->OnDestroy(); return true; }
                return false;
            }),
        m_gameObjects.end());
}

void Scene::OnEnter() 
{
    GameObject* camObj = CreateGameObject("MainCamera");
    camObj->GetTransform()->SetPosition({ 0.f, 3.f, -8.f });
    camObj->GetTransform()->SetRotation({ 15.f, 0.f, 0.f });
    GameEngine::Instance()->SetGameCamera(camObj->AddComponent<Camera>());
}

void Scene::OnExit()
{
    m_gameObjects.clear();
    m_awakeQueue.clear();
    m_startQueue.clear();
    m_fixed.clear();
}

GameObject* Scene::CreateGameObject(const std::string& name)
{
    auto newObj = std::make_unique<GameObject>(this, name);
    GameObject* ptr = newObj.get();
    m_gameObjects.push_back(std::move(newObj));
    m_awakeQueue.push(ptr);
    return ptr;
}

void Scene::RegisterForRigidbody(Rigidbody* rigid)
{
    if (std::find(m_fixed.begin(), m_fixed.end(), rigid) == m_fixed.end())
        m_fixed.emplace_back(rigid);
}

void Scene::UnRegisterForRigidbody(Rigidbody* rigid)
{
    m_fixed.erase(
        std::remove(m_fixed.begin(), m_fixed.end(), rigid),
        m_fixed.end());
}

bool Scene::IsPlaying() const
{
    return GameEngine::Instance()->GetGameApp()->IsPlay();
}

// Play 시작 시 모든 컴포넌트 OnEnable 재호출
void Scene::OnPlayStart()
{
    for (auto& obj : m_gameObjects)
    {
        if (!obj->GetActive()) continue;

        for (auto& comp : obj->GetComponents())
        {
            if (!comp->IsEnabled()) continue;

            // 아직 활성화 안 된 컴포넌트만 활성화
            // (에디터에서 추가했지만 IsPlaying=false라 OnEnable 못 탄 것들)
            if (!comp->GetActive())
            {
                comp->SetActive(true);
                comp->OnEnable();
            }
        }
    }
}

//핵심 수정 3: Play 정지 시 물리 상태 초기화 
void Scene::OnPlayStop()
{
    // Rigidbody 속도 초기화
    for (auto* rigid : m_fixed)
        rigid->SetVelocity({0, 0, 0});

  
}