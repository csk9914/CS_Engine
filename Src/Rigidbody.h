#pragma once
#include "Component.h"
#include "Vector3.h"

// 누적된 힘 → 가속도 → 속도 → 위치 변위
class Rigidbody : public  Component
{
public:
	Rigidbody() : Component("Rigidbody") {}

	void AddForce(Vector3 force);

	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void OnDestroy() override;

	virtual void FixedUpdate(float dt);

	virtual void OnEditorGUI() override;

public:
	void SetMass(float mass) { m_mass = (mass <= 0.0f) ? 0.1f : mass; }
	void SetUseGravity(bool use) { m_useGravity = use; }
	void SetVelocity(const Vector3& vel) { m_velocity = vel; }

	// 회전력 추가 함수
	void AddTorque(const Vector3& torque) { m_torqueSum += torque; }

	// 각속도 제어
	void SetAngularVelocity(const Vector3& angVel) { m_angularVelocity = angVel; }
	Vector3 GetAngularVelocity() const { return m_angularVelocity; }

	bool IsKinematic() const { return m_isKinematic; }
	Vector3  GetVelocity() const { return m_velocity; }

private:
	// 운동 관련 변수
	Vector3 m_velocity = {};				// 현재 속도 (m/s)
	Vector3 m_acceleration = {};			// 현재 가속도
	Vector3 m_forceSum = {};				// 이번 프레임에 누적된 힘의 합

	// 물리 속성
	float m_mass = 1.0f;					// 질량 (F=ma)
	float m_drag = 5.0f;					// 공기 저항 (속도를 줄임)
	float m_angularDrag = 0.05f;			// 회전 저항 

	// 상태 제어
	bool m_useGravity = true;
	bool m_isKinematic = false;				// 물리 엔진이 아닌 코드로 직접 움직일지 여부
	bool m_freezeRotationX = false;			// 축 회전 고정 (유니티 Constraints 기능)
	bool m_freezeRotationY = false;
	bool m_freezeRotationZ = false;

	// 회전 관련 변수 추가
	Vector3 m_angularVelocity = {};      // 현재 회전 속도 (도/s 또는 라디안/s)
	Vector3 m_torqueSum = {};            // 누적된 회전력(Torque)

	static inline Vector3 s_gravity = { 0.0f, -9.8f, 0.0f };
};