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
#include "Rigidbody.h"

void MainScene::OnEnter()
{
	Scene::OnEnter();

	GameObject* plane = CreatePrimitive(PrimitiveType::Plane);
	plane->GetTransform()->SetPosition({0.f, -1.f, 0.f});
	plane->GetTransform()->SetScale({ 10.f, 1.f, 10.f });

	GameObject* cube = CreatePrimitive(PrimitiveType::Cube);
	cube->GetComponent<MeshRenderer>()->SetColor(1.0f, 0.f, 0.f, 1.0f);
	cube->AddComponent<Rigidbody>();
}

void MainScene::Update(float deltaTime)
{

	Scene::Update(deltaTime);
}

void MainScene::OnExit()
{

}
