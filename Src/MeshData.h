#pragma once

#include <windows.h>
#include <vector>
#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

    // 메쉬의 최소/최대 좌표를 계산해서 반환
    void GetBounds(DirectX::XMFLOAT3& outMin, DirectX::XMFLOAT3& outMax) const
    {
        if (vertices.empty()) return;

        outMin = { FLT_MAX, FLT_MAX, FLT_MAX };
        outMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

        for (const auto& v : vertices)
        {
            outMin.x = (std::min)(outMin.x, v.position.x);
            outMin.y = (std::min)(outMin.y, v.position.y);
            outMin.z = (std::min)(outMin.z, v.position.z);

            outMax.x = (std::max)(outMax.x, v.position.x);
            outMax.y = (std::max)(outMax.y, v.position.y);
            outMax.z = (std::max)(outMax.z, v.position.z);
        }
    }
};