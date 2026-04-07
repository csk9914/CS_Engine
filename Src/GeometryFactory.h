#pragma once
#include "MeshData.h"

class GeometryFactory
{
public:
	// 사각형 (기본 단위)
	static MeshData CreateRectangle(float width, float height);

	static MeshData CreatePlane(float size);

	// 큐브
	static MeshData CreateCube(float size);

	// 구
	static MeshData CreateSphere(float radius, UINT sliceCount, UINT stackCount);
};