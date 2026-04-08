#pragma once
#include "Collider.h"

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider(const std::string& name = "CapsuleCollider") : Collider(ColliderType::Capsule, name) {}

	// 데이터 설정
	void SetRadius(float radius) { m_radius = radius; }
	void SetHeight(float height) { m_height = height; }	// 원통 부분의 순수 노ㅠㅍ이

	float GetRadius() const { return m_radius; }
	float GetHeight() const { return m_height; }

	// 충돌 계산을 위한 선분(양 끝점) 좌표 계산
	void GetSegment(Vector3& outTop, Vector3& outBottom);

private:
	float m_radius = 0.5f;
	float m_height = 1.0f;
};