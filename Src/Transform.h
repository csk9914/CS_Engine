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

	virtual void OnEditorGUI() override;

private:
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_scale;

};