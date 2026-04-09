#pragma once
#include "MeshData.h"

#include <wrl/client.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

#include "Object.h"

class Mesh : public Object
{
public:
	Mesh(const std::string name = "Mesh") : Object(name) {}
	Mesh(const MeshData& data, const std::string name = "Mesh");

public:
	// 어떤 데이터가 들어오든 GPU 버퍼로 변환
	bool Create(const MeshData& data);

	void Bind(ID3D11DeviceContext* ctx);

	UINT GetIndexCount() const { return m_indexCount; }

	void GetBounds(DirectX::XMFLOAT3& min, DirectX::XMFLOAT3& max) { min = m_min; max = m_max; }

private:
	UINT m_indexCount = 0;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_iBuffer;

	DirectX::XMFLOAT3 m_min = {};
	DirectX::XMFLOAT3 m_max = {};
};