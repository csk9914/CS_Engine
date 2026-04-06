#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

class DX11Renderer;
class GameObject;
class Camera;

class Gizmo
{
public:
	enum class Axis { None, X, Y, Z };

	Gizmo();
	~Gizmo();

	bool Init(DX11Renderer* render);

	// 매 프레임 호출
	void Update(float deltaTime);

	void Render();

	void SetTarget(GameObject* obj) { m_target = obj; }
	GameObject* GetTarget() const { return m_target; }

private:
	// Ray cast — 마우스 → 월드 레이 변환
	void CalcMouseRay(DirectX::XMVECTOR& outOrigin, DirectX::XMVECTOR& outDir) const;

	// 축 화살표와 레이의 교차 판정
	Axis PickAxis(const DirectX::XMVECTOR& rayOrigin, const DirectX::XMVECTOR& rayDir) const;

	// 드래그 이동량 계산 (축 방향 평면에 레이 투영)
	float CalcDragDelta(const DirectX::XMVECTOR& rayOrigin, const DirectX::XMVECTOR& rayDir, Axis axis) const;

	// 화살표 한 개 그리기
	void DrawArrow(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT4& color);

	bool InitShader(DX11Renderer* render);
	bool InitArrowBuffers(DX11Renderer* render);
	bool InitCBuffers(DX11Renderer* render);

private:
	// DX11 리소스
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vb;				// 라인 버텍스 (2개)
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbMatrix;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbColor;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_inputLayout;

	bool m_initialized = false;

	// 상태
	GameObject* m_target = nullptr;
	Axis m_activeAxis = Axis::None;  // 드래그 중인 축
	bool m_isDragging = false;

	// 드래그 시작 시점의 레이 정보 (이동량 계산용)
	DirectX::XMVECTOR m_dragStartRayOrigin = {};
	DirectX::XMVECTOR m_dragStartRayDir = {};
	float m_dragStartT = 0.f;  // 드래그 시작 시 투영 거리

	// 화살표 길이 / 두께 (화면 크기에 무관하게 고정)
	static constexpr float ARROW_LENGTH = 1.5f;
	static constexpr float AXIS_HIT_RADIUS = 0.5f; // 클릭 판정 반경
};