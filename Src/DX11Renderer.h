#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>   // ComPtr
#include <memory>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib") // 나중에 쉐이더 쓸 때 필요함
#pragma comment(lib, "dxgi.lib")

#include "IRenderer.h"

class DX11Renderer : public IRenderer
{
private:
	DX11Renderer() = default;
	bool Init(HWND hWnd, int width, int height);
	bool CreateDepthBuffer(int width, int height);

public:
	~DX11Renderer();

	// 팩토리 함수 (기존 Renderer::CreateRenderer 패턴 유지)
	static std::unique_ptr<DX11Renderer> Create(HWND hWnd, int width, int height);

	// IRenderer 구현
	virtual void Flip() override;   // Present

	// 매 프레임 시작 시 호출
	void BeginFrame(float r = 0.1f, float g = 0.1f, float b = 0.1f);

	// 셰이더 컴파일 헬퍼
	bool CompileShader(const wchar_t* filePath, const char* entry,
		const char* target, ID3DBlob** ppBlob);

public:
	// DX11 자원 접근 
	ID3D11Device* GetDevice()  const { return m_device.Get(); }
	ID3D11DeviceContext* GetContext() const { return m_context.Get(); }

	int GetWidth()  const { return m_width; }
	int GetHeight() const { return m_height; }

private:
	Microsoft::WRL::ComPtr<ID3D11Device>           m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain>         m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;

	D3D11_VIEWPORT m_viewport = {};

	int m_width = 0;
	int m_height = 0;
};