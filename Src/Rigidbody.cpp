#include "Rigidbody.h"
#include "EditorUI.h"
#include "GameObject.h"
#include "Scene.h"

void Rigidbody::AddForce(Vector3 force)
{
	// 힘을 누적
	m_velocity += force;
}

void Rigidbody::OnEnable()
{
	GetGameObject()->GetScene()->RegisterForRigidbody(this);
}

void Rigidbody::OnDisable()
{
	GetGameObject()->GetScene()->UnRegisterForRigidbody(this);
}

void Rigidbody::OnDestroy()
{
	GetGameObject()->GetScene()->UnRegisterForRigidbody(this);
}

void Rigidbody::FixedUpdate(float dt)
{
	// kinematic 상태라면 물리 연산을 하지 않음
	if (m_isKinematic)
	{
		m_forceSum = {}; return;
	}

	// 중력 적용
	if (m_useGravity)
	{
		// F = mg
		m_forceSum += m_mass * s_gravity;
	}

	// 가속도 계산 (F = ma ->  a= F/m)
	m_acceleration = m_forceSum / m_mass;

	m_velocity += m_acceleration * dt;

	// 공기 저항 적용
	// 속도에 비례하여 감속
	m_velocity = m_velocity * (1.0f - m_drag * dt);

	// 실제 위치 반영
	// 속도가 미세하면 움직이지 않게 처리(floating point error 방지)
	if (m_velocity.Length() > 0.001f)
	{
		GetGameObject()->GetTransform()->AddPosition(m_velocity * dt);
	}

	// 7. 힘 초기화 (다음 프레임을 위해)
	m_forceSum = { 0, 0, 0 };
}

void Rigidbody::OnEditorGUI()
{

	// 1. 질량 (0보다 커야 함)
	if (ImGui::DragFloat("Mass", &m_mass, 0.1f, 0.1f, 1000.0f))
	{
		if (m_mass < 0.1f) m_mass = 0.1f;
	}

	// 2. 저항력
	ImGui::DragFloat("Drag", &m_drag, 0.01f, 0.0f, 10.0f);

	// 3. 중력 및 키네매틱
	ImGui::Checkbox("Use Gravity", &m_useGravity);
	ImGui::Checkbox("Is Kinematic", &m_isKinematic);

	// 4. 현재 속도 (읽기 전용 혹은 디버깅용 수동 수정)
	float vel[3] = { m_velocity.x, m_velocity.y, m_velocity.z };
	if (ImGui::DragFloat3("Velocity", vel, 0.1f))
	{
		m_velocity = { vel[0], vel[1], vel[2] };
	}

	// 5. 제약 사항 (Constraints)
	if (ImGui::TreeNode("Constraints"))
	{
		ImGui::Checkbox("Freeze Rotation X", &m_freezeRotationX);
		ImGui::Checkbox("Freeze Rotation Y", &m_freezeRotationY);
		ImGui::Checkbox("Freeze Rotation Z", &m_freezeRotationZ);
		ImGui::TreePop();
	}

}
