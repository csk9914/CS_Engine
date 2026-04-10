#pragma once
#include "Component.h"
#include "Vector3.h"
#include <DirectXMath.h>


class Transform : public Component
{
public:
	Transform(const std::string& name = "Transform") : m_position(0, 0, 0), m_rotation(0, 0, 0), m_scale(1.f, 1.f, 1.f) {}
	virtual ~Transform() {}

public:
	void AddPosition(Vector3 addPos);

public:
	const Vector3& GetPosition() const { return m_position; }
	const Vector3& GetRotation() const { return m_rotation; }
	const Vector3& GetScale() const { return m_scale; }


	void SetPosition(const Vector3& position) { m_position = position; }
	void SetRotation(const Vector3& rotation) { m_rotation = rotation; }
	void SetScale(const Vector3& scale) { m_scale = scale; }

	// 행렬 변환
	DirectX::XMMATRIX GetWorldMatrix() const;

	void SetRotationFromQuaternion(DirectX::XMVECTOR quat);

public:
	// 유니티 스타일 방향 벡터 (World Space)
	Vector3 GetForward() const;
	Vector3 GetRight() const;
	Vector3 GetUp() const;

	// LookAt: 타겟 지점을 바라보도록 회전값 수정
	void LookAt(const Vector3& target);

	// 좌표계 변환 (Local <-> World)
	Vector3 TransformPoint(const Vector3& localPoint);
	Vector3 InverseTransformPoint(const Vector3& worldPoint);

	// 부모-자식 구조가 있다면 추가될 부분 (현재는 단일 객체 기준)
	// DirectX::XMMATRIX GetLocalMatrix() const;

	virtual void OnEditorGUI() override;

private:
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_scale;

};