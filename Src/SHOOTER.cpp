#include "SHOOTER.h"
#include "GameObject.h"
#include "Component.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "GameEngine.h"
#include "GeometryFactory.h"
#include "MeshFilter.h"
#include "ResourceManager.h"

SHOOTER::SHOOTER()
{
    m_appConfig.title           = "CS Engine Editor";
    m_appConfig.width           = 1280;
    m_appConfig.height          = 720;
    m_appConfig.fps             = 60.0f;
    m_appConfig.isCursorVisible = true;
}

void SHOOTER::Start()
{
    GameObject* camObj = m_scene.CreateGameObject("MainCamera");
    camObj->GetTransform()->SetPosition({0.f, 3.f, -8.f});
    camObj->GetTransform()->SetRotation({15.f, 0.f, 0.f});
    GameEngine::Instance()->SetGameCamera(camObj->AddComponent<Camera>()); 


  
    auto makeCube = [&](const std::string& name, Vector3 pos,
                        float r, float g, float b)
    {
        GameObject* obj = m_scene.CreateGameObject(name);
        obj->GetTransform()->SetPosition(pos);

        // 메쉬 생성 ( resource 매니저에서 관리)
        Mesh* mesh = ResourceManager::Instance()->GetMesh("CSCube");
        obj->AddComponent<MeshFilter>()->SetMesh(mesh);
        obj->AddComponent<MeshRenderer>()->SetColor(r, g, b);

        return obj;
    };

    GameObject* obj = m_scene.CreateGameObject("plane");
    obj->GetTransform()->SetPosition({0, -2, 0});

    // 메쉬 생성 ( resource 매니저에서 관리)
    Mesh* mesh = ResourceManager::Instance()->GetMesh("CSPlane");

    obj->AddComponent<MeshFilter>()->SetMesh(mesh);
    obj->AddComponent<MeshRenderer>();

    makeCube("Cube_Red",   {-3.f, 0.f, 0.f}, 0.9f, 0.3f, 0.3f);
    makeCube("Cube_Green", { 0.f, 0.f, 0.f}, 0.3f, 0.9f, 0.3f);
    makeCube("Cube_Blue",  { 3.f, 0.f, 0.f}, 0.3f, 0.5f, 0.9f);

    m_scene.OnEnter();
}

void SHOOTER::Update(float dt)
{
    //static float angle = 0.f;
    //angle += 30.f * dt;

    //for (auto& obj : m_scene.GetGameObjects())
    //{
    //    if (obj->GetName() == "MainCamera") continue;
    //    Vector3 rot = obj->GetTransform()->GetRotation();
    //    rot.y = angle;
    //    obj->GetTransform()->SetRotation(rot);
    //}

    m_scene.Update(dt);
}

void SHOOTER::End()
{
    m_scene.OnExit();
}
