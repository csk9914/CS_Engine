#pragma once
#include "MeshData.h"

#include <wrl/client.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

class Mesh
{
public:
	Mesh() {}
	Mesh(const MeshData& data);

public:
	// 어떤 데이터가 들어오든 GPU 버퍼로 변환
	bool Create(const MeshData& data);

	void Bind(ID3D11DeviceContext* ctx);

	UINT GetIndexCount() const { return m_indexCount; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_iBuffer;
	UINT m_indexCount = 0;
};