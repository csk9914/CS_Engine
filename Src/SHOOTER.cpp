#include "SHOOTER.h"

#include "GameObject.h"
#include "Component.h"
#include "SpriteRenderer.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "GameEngine.h"

SHOOTER::SHOOTER()
{
	m_appConfig.title = "SHOOTER";
	m_appConfig.width = 1280;
	m_appConfig.height = 720;
	m_appConfig.fps = 60.0f;
	m_appConfig.isCursorVisible = false;
}

void SHOOTER::Start()
{
	// 에디터 카메라
	GameObject* camObj = m_scene.AddGameObject("MainCamera");
	camObj->GetTransform()->SetPosition({ 0.f, 3.f, -8.f });
	camObj->GetTransform()->SetRotation({ 15.f, 0.f, 0.f });
	Camera* cam = camObj->AddComponent<Camera>();
	GameEngine::Instance()->SetMainCamera(cam);
	
	// 큐브 3개
	auto makeCube = [&](const std::string& name, Vector3 pos, float r, float g, float b)
		{
			GameObject* obj = m_scene.AddGameObject(name);
			obj->GetTransform()->SetPosition(pos);
			MeshRenderer* mr = obj->AddComponent<MeshRenderer>();
			mr->SetColor(r, g, b);
			return obj;
		};

	makeCube("Cube_Red", { -3.f, 0.f, 0.f }, 0.9f, 0.3f, 0.3f);
	makeCube("Cube_Green", { 0.f, 0.f, 0.f }, 0.3f, 0.9f, 0.3f);
	makeCube("Cube_Blue", { 3.f, 0.f, 0.f }, 0.3f, 0.5f, 0.9f);

	m_scene.OnEnter();

}

void SHOOTER::Update(float deltaTime)
{
	// 큐브를 천천히 회전시켜 렌더링 확인
	static float angle = 0.f;
	angle += 30.f * deltaTime; // 초당 45도

	const auto& objs = m_scene.GetGameObjects();
	for (auto& obj : objs)
	{
		if (obj->GetName() == "MainCamera") 
			continue;
		Vector3 rot = obj->GetTransform()->GetRotation();
		rot.y = angle;
		obj->GetTransform()->SetRotation(rot);
	}

	m_scene.Update(deltaTime);
}


void SHOOTER::End()
{
	m_scene.OnExit();
}
