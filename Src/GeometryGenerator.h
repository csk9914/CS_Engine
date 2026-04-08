#pragma once
#include "MeshData.h"
#include "PrimitiveType.h"

class GeometryGenerator
{
public:
	// 사각형 (기본 단위)
	static MeshData CreateRectangle(float width, float height);

	static MeshData CreatePlaneData(float size);

	// 큐브
	static MeshData CreateCubeData(float size);

	// 구
	static MeshData CreateSphereData(float radius, UINT sliceCount, UINT stackCount);
};