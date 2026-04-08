#pragma once
#include "Component.h"
#include <DirectXMath.h>
#include "Vector2.h"
#include <Vector3.h>

struct Ray
{
	Vector3 origin;
	Vector3 direction;
};

class Camera : public Component
{
public:
	Camera(const std::string& name = "Camera");
	virtual ~Camera() {}

	virtual void Update(float DeltaTime) override;

	virtual void OnEnable() override;

	virtual void OnDisable() override;

public:
	// 화면 좌표(Screen Space)를 받아서 월드 레이를 반환
	// screenPos: 씬 뷰 내부의 0~1 혹은 픽셀 좌표
	// viewSize: 씬 뷰의 가로세로 크기
	Ray ScreenPointToRay(Vector2 screenPos, Vector2 viewSize);

	void UpdateMatrix(float aspext);

	// GPU의 상수 버퍼에 카메라 행렬(View, Proj)을 전달하는 함수
	void BindToGPU();

	virtual void OnEditorGUI() override;

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
	float m_zoomSpeed = 10.f;

private:
	DirectX::XMMATRIX m_view = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX m_proj = DirectX::XMMatrixIdentity();

};
