#include "SceneManager.h"
#include "SceneMachine.h"
#include "GameEngine.h"
#include "GameApp.h"
#include "GameObject.h"

void SceneManager::LoadScene(std::unique_ptr<Scene> newScene)
{

    auto* app = GameEngine::Instance()->GetGameApp();
    if (app)
    {
        app->SetFirstScene(std::move(newScene));
    }

}

Scene* SceneManager::GetActiveScene()
{
	auto* app = GameEngine::Instance()->GetGameApp();
	return (app) ? app->GetCurrentScene() : nullptr;
}

GameObject* SceneManager::Instantiate(const std::string& name)
{
    // 현재 활성화된 씬을 가져와서 대신 생성해줌
    Scene* activeScene = GetActiveScene();
    if (activeScene)
    {
        return activeScene->CreateGameObject(name);
    }
    return nullptr;
}
