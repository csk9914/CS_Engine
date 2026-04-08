#pragma once
#include <memory>
#include "Scene.h"

class GameObject;

class SceneManager
{
public:
	static void LoadScene(std::unique_ptr<Scene> newScene);

	// 유니티의 SceneManager.GetActiveScene()과 동일
	static Scene* GetActiveScene();

	static GameObject* Instantiate(const std::string& name = "GameObject");
};