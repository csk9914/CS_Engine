#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include "IRenderer.h"
#include "MeshRenderer.h"
#include "Rigidbody.h"
#include "GameEngine.h"
#include "GameApp.h"
#include "CollisionManager.h"

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

    for (auto* rb : m_rigids)
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

void Scene::OnEnter() {}

void Scene::OnExit()
{
    m_gameObjects.clear();
    m_awakeQueue.clear();
    m_startQueue.clear();
    m_rigids.clear();
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
    if (std::find(m_rigids.begin(), m_rigids.end(), rigid) == m_rigids.end())
        m_rigids.emplace_back(rigid);
}

void Scene::UnRegisterForRigidbody(Rigidbody* rigid)
{
    m_rigids.erase(
        std::remove(m_rigids.begin(), m_rigids.end(), rigid),
        m_rigids.end());
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

// ── 핵심 수정 3: Play 정지 시 물리 상태 초기화 ───────────────────────
void Scene::OnPlayStop()
{
    // Rigidbody 속도 초기화
    for (auto* rb : m_rigids)
        rb->SetVelocity({ 0, 0, 0 });

    // TODO: 나중에 씬 직렬화 구현 시 Transform 위치도 원복
}