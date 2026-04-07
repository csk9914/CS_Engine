#include "GameScene.h"
#include "GameObject.h"
#include "Component.h"
#include "IRenderer.h"
#include "MeshRenderer.h"


#include "MeshData.h"
#include "GeometryFactory.h"
#include "Mesh.h"
#include "MeshFilter.h"

GameScene::~GameScene()
{
}

void GameScene::Update(float deltaTime)
{
	// 모든 객체 업데이트
	for (auto& obj : m_gameObjects)
	{
		obj->Update(deltaTime);
	}


	// 죽은 객체만 골라서 삭제 (백터에서 제거)
	m_gameObjects.erase(std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
		[](const std::unique_ptr<GameObject>& obj)
		{
			if (obj->IsDestroy()) { obj->OnDestroy(); return true; }
			return false;
		}),
		m_gameObjects.end());
}

void GameScene::OnEnter()
{

}

void GameScene::OnExit()
{

}

GameObject* GameScene::CreateGameObject(const std::string& name )
{
	auto newObj = std::make_unique<GameObject>(name);
	GameObject* ptr = newObj.get();

	m_gameObjects.push_back(std::move(newObj));

	return ptr;
}

GameObject* GameScene::CreatePrimitive(PrimitiveType type)
{
    // 1. 빈 객체 생성 (이름은 타입에 맞게)
    std::string name = "Primitive";
    MeshData data;

    // 2. 타입에 따른 데이터 가져오기
    switch (type)
    {
    case PrimitiveType::Cube:
        name = "Cube";
        data = GeometryFactory::CreateCube(1.0f);
        break;
    case PrimitiveType::Plane:
        name = "Plane";
        data = GeometryFactory::CreateRectangle(1.0f, 1.0f);
        break;
        // ... Sphere 등 추가
    }

    GameObject* obj = CreateGameObject(name);

    // 3. 메쉬 리소스 생성 및 굽기 (나중에는 ResourceManager에서 가져오도록 개선 가능)
    Mesh* mesh = new Mesh();
    mesh->Create(data);

    // 4. 컴포넌트 일괄 세팅
    auto* filter = obj->AddComponent<MeshFilter>();
    filter->SetMesh(mesh);

    obj->AddComponent<MeshRenderer>();

    return obj;
}
