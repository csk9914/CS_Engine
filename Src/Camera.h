#pragma once
#include "Component.h"
#include <DirectXMath.h>


class Camera : public Component
{
public:
	Camera();
	virtual ~Camera() {}

	virtual void Update(float DeltaTime) override;

	virtual void OnEnable() override;

	virtual void OnDisable() override;

private:
	void UpdateMatrix(float aspext);

public:
	// 행렬 접근
	DirectX::XMMATRIX GetViewMatrix() const { return m_view; }
	DirectX::XMMATRIX GetProjectionMatrix() const { return m_proj; }

public:
	// Inspector에서 수정 가능하도록 public
	float m_fov = 60.f;
	float m_nearZ = 0.1f;
	float m_farZ = 1000.f;
	float m_moveSpeed = 10.f;
	float m_rotateSpeed = 30.f;
	float m_panSpeed = 10.f;
	float m_zoomSpeed = 300.f;

private:
	DirectX::XMMATRIX m_view = {};
	DirectX::XMMATRIX m_proj = {};

};
