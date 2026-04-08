#include "MainScene.h"
#include "GameObject.h"
#include "Camera.h"
#include "GameEngine.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"

void MainScene::OnEnter()
{
    GameObject* camObj = CreateGameObject("MainCamera");
    camObj->GetTransform()->SetPosition({ 0.f, 3.f, -8.f });
    camObj->GetTransform()->SetRotation({ 15.f, 0.f, 0.f });
    GameEngine::Instance()->SetGameCamera(camObj->AddComponent<Camera>());

    GameObject* plane = CreatePrimitive(PrimitiveType::Plane);
    GameObject* cube = CreatePrimitive(PrimitiveType::Cube);
    GameObject* sphere = CreatePrimitive(PrimitiveType::Sphere);
}

void MainScene::OnExit()
{
}
