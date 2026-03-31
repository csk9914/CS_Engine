#include "SHOOTER.h"

#include "GameObject.h"
#include "Component.h"
#include "SpriteRenderer.h"

SHOOTER::SHOOTER()
{
	m_appConfig.width = 120;
	m_appConfig.height = 34;
	m_appConfig.title = "SHOOTER";
	m_appConfig.fps = 60.0f;
	m_appConfig.isCursorVisible = false;

	m_player = new GameObject();
}

void SHOOTER::Start()
{
	m_player->AddComponent<SpriteRenderer>();
	m_player->GetTransform()->SetPosition({ 10.0f, 10.0f });
	m_player->GetComponent<SpriteRenderer>()->SetSprite("A");
}

void SHOOTER::Update(float deltaTime)
{
	m_player->Update(deltaTime);
}



void SHOOTER::End()
{
	delete m_player;
}
