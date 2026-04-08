#include "SHOOTER.h"
#include "GameObject.h"
#include "Component.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "GameEngine.h"
#include "GeometryGenerator.h"
#include "MeshFilter.h"
#include "ResourceManager.h"
#include "MainScene.h"

SHOOTER::SHOOTER()
{
    m_appConfig.title = "Shooter";
    m_appConfig.width = 1280;
    m_appConfig.height = 720;
    m_appConfig.fps = 60.0f;
    m_appConfig.isCursorVisible = true;
}

void SHOOTER::Init()
{
    SetFirstScene(std::make_unique<MainScene>());
}

