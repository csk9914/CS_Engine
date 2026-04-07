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
};