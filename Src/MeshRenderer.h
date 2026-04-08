#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <vector>
#include <string>

#include "Component.h"

// cbuffer 구조체 — HLSL의 CBMatrix와 메모리 레이아웃 일치
struct CBMatrix
{
    DirectX::XMMATRIX World;
    DirectX::XMMATRIX View;
    DirectX::XMMATRIX Projection;
};

struct CBColor
{
    DirectX::XMFLOAT4 Color;
};

class MeshRenderer : public Component
{
public:
    MeshRenderer();
    virtual ~MeshRenderer();

    // Component 생명주기
    virtual void Awake() override;

    virtual void OnEnable()  override;   // RenderManager에 등록
    virtual void OnDisable() override;   // RenderManager에서 해제
    virtual void OnDestroy() override;   // RenderManager에서 해제

    // RenderManager가 호출
    virtual void Render();

    // 색상 설정
    void SetColor(float r, float g, float b, float a = 1.f) 
    { 
        m_color = { r, g, b, a }; 
    }

    DirectX::XMFLOAT4 GetColor() const { return m_color; }

private:
    bool InitShader(); // 셰이더 컴파일 + InputLayout
    bool InitCBuffers();

private:


    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbMatrix;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbColor;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_ps;

    Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_inputLayout;


    DirectX::XMFLOAT4 m_color = { 1.f, 1.f, 1.f, 1.f };

    bool m_initialized = false;
};