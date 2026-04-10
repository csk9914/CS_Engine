#include "PlayerMove.h"

#include "ComponentRegistry.h"
#include "..\..\InputHandler.h"
#include "GameObject.h"

void PlayerMove::Start()
{
	m_transform = gameObject()->GetTransform();
	m_rigid = gameObject()->GetComponent<Rigidbody>();

	m_speed = 20;
	m_rotSpeed = 50;
}

void PlayerMove::Update(float dt)
{
	auto input = InputHandler::Instance();

	// 1. 이동 입력
	float moveInput = 0.0f;
	if (input->IsKeyDown('W')) moveInput += 1.0f;
	if (input->IsKeyDown('S')) moveInput -= 1.0f;

	// 현재 속도 체크
	Vector3 currentVel = m_rigid->GetVelocity();
	float maxSpeed = 100.0f; // 탱크의 최대 속도 제한

	// 현재 속도가 최대치보다 낮을 때만 힘을 가함
	if (currentVel.Length() < maxSpeed)
	{
		m_rigid->AddForce(m_transform->GetForward() * moveInput * m_speed);
	}

	// 2. 회전 입력
	float rotInput = 0.0f;
	if (input->IsKeyDown('D')) rotInput += 1.0f;
	if (input->IsKeyDown('A')) rotInput -= 1.0f;

	// 각속도(회전 속도) 제한
	Vector3 currentAngVel = m_rigid->GetAngularVelocity();
	float maxRotSpeed = 100.0f; // 최대 회전 속도 제한

	if (currentAngVel.Length() < maxRotSpeed)
	{
		m_rigid->AddTorque(Vector3(0, 1, 0) * rotInput * m_rotSpeed);
	}

}

REGISTER_COMPONENT(PlayerMove, "Scripts")