#include "Rigidbody.h"
#include "EditorUI.h"
#include "GameObject.h"
#include "Scene.h"

#include "ComponentRegistry.h"

void Rigidbody::AddForce(Vector3 force)
{
	// 힘을 누적
	m_forceSum += force;
}

void Rigidbody::OnEnable()
{
	gameObject()->GetScene()->RegisterForRigidbody(this);
}

void Rigidbody::OnDisable()
{
	gameObject()->GetScene()->UnRegisterForRigidbody(this);
}

void Rigidbody::OnDestroy()
{
	gameObject()->GetScene()->UnRegisterForRigidbody(this);
}

void Rigidbody::FixedUpdate(float dt)
{
    if (m_isKinematic)
    {
        m_forceSum = {}; m_torqueSum = {}; return;
    }

    // 선형 운동 (기존 로직) 
    if (m_useGravity) m_forceSum += m_mass * s_gravity;
    m_acceleration = m_forceSum / m_mass;
    m_velocity += m_acceleration * dt;
    m_velocity *= (1.0f - m_drag * dt); // 선형 저항

    if (m_velocity.Length() > 0.001f)
    {
        gameObject()->GetTransform()->AddPosition(m_velocity * dt);
    }

    // 회전 운동 
    // 각가속도 계산 (단순화: 관성 모멘트를 무시하고 질량 비례로 계산하거나 1.0으로 가정)
    Vector3 angularAcceleration = m_torqueSum / m_mass;

    // 각속도 갱신
    m_angularVelocity += angularAcceleration * dt;

    // 회전 저항 (Angular Drag) 적용
    m_angularVelocity *= (1.0f - m_angularDrag * dt);

    // 실제 회전 반영 (Constraints 체크)
    Vector3 deltaRotation = m_angularVelocity * dt;
    if (deltaRotation.Length() > 0.001f)
    {
        Vector3 currentRot = gameObject()->GetTransform()->GetRotation();

        if (!m_freezeRotationX) currentRot.x += deltaRotation.x;
        if (!m_freezeRotationY) currentRot.y += deltaRotation.y;
        if (!m_freezeRotationZ) currentRot.z += deltaRotation.z;

        gameObject()->GetTransform()->SetRotation(currentRot);
    }

    // 초기화 
    m_forceSum = { 0, 0, 0 };
    m_torqueSum = { 0, 0, 0 }; // 토크 초기화
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

REGISTER_COMPONENT(Rigidbody, "Rigidbody")