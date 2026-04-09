#include "MainScene.h"
#include "GameObject.h"
#include "Camera.h"
#include "GameEngine.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CapsuleCollider.h"

void MainScene::OnEnter()
{
	GameObject* camObj = CreateGameObject("MainCamera");
	camObj->GetTransform()->SetPosition({ 0.f, 3.f, -8.f });
	camObj->GetTransform()->SetRotation({ 15.f, 0.f, 0.f });
	GameEngine::Instance()->SetGameCamera(camObj->AddComponent<Camera>());

	//CreatePrimitive(PrimitiveType::Plane)
	//	->GetTransform()->SetPosition({ 0.f, -1.f, 0.f });

	GameObject* cube = CreatePrimitive(PrimitiveType::Cube);
	cube->GetTransform()->SetPosition({ -2,0,0 });
	cube->GetComponent<MeshRenderer>()->SetColor(1, 0.1, 0.1, 1);


	//GameObject* sphere = CreatePrimitive(PrimitiveType::Sphere);
	//sphere->GetComponent<MeshRenderer>()->SetColor(0.f, 1.f, 0.f, 1.f);
	//sphere->AddComponent<SphereCollider>()->SetRadius(0.5f);

	//GameObject* capsule = CreatePrimitive(PrimitiveType::Capsule);
	//capsule->GetTransform()->SetPosition({ 0, 2, 0 });
	//capsule->GetComponent<MeshRenderer>()->SetColor(0.f, 1.f, 0.f, 1.f);
	//auto capCol = capsule->AddComponent<CapsuleCollider>();
	//capCol->SetRadius(0.5f);
	//capCol->SetHeight(1.0f);

	GameObject* cube2 = CreatePrimitive(PrimitiveType::Cube);
	cube2->SetName(std::string("cube2"));
	cube2->GetTransform()->SetPosition({ 2,0,0 });
	cube2->GetComponent<MeshRenderer>()->SetColor(0, 0.3, 1, 1);


	m_moveObj = cube2;
}

void MainScene::Update(float deltaTime)
{

	Scene::Update(deltaTime);
}

void MainScene::OnExit()
{

}
