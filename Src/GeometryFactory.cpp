#include "GeometryFactory.h"

MeshData GeometryFactory::CreateRectangle(float width, float height)
{

	return MeshData();
}

MeshData GeometryFactory::CreatePlane(float size)
{
	MeshData data;
	float s = size * 0.5f;

	data.vertices = {
		// 윗면
		{{-s, 0, s}, {0, 1, 0}, {0, 0}}, {{s, 0, s}, {0, 1, 0}, {1, 0}},
		{{-s, 0, -s}, {0, 1, 0}, {0, 1}}, {{s, 0, -s}, {0, 1, 0}, {1, 1}}
	};

	// 인덱스 (시계 방향)
	data.indices = { 0, 1, 2, 2, 1, 3 };
	return data;
}

MeshData GeometryFactory::CreateCube(float size)
{
	MeshData data;
	float s = size * 0.5f;

	// 정점 정의 (Position, Normal, UV)
	data.vertices = {
		// 앞면
		{{-s, s, -s}, {0, 0, -1}, {0, 0}}, {{s, s, -s}, {0, 0, -1}, {1, 0}},
		{{-s, -s, -s}, {0, 0, -1}, {0, 1}}, {{s, -s, -s}, {0, 0, -1}, {1, 1}},

		// 뒷면
		{{s, s, s}, {0, 0, 1}, {0, 0}}, {{-s, s, s}, {0, 0, 1}, {1, 0}},
		{{s, -s, s}, {0, 0, 1}, {0, 1}}, {{-s, -s, s}, {0, 0, 1}, {1, 1}},

		// 윗면
		{{-s, s, s}, {0, 1, 0}, {0, 0}}, {{s, s, s}, {0, 1, 0}, {1, 0}},
		{{-s, s, -s}, {0, 1, 0}, {0, 1}}, {{s, s, -s}, {0, 1, 0}, {1, 1}},

		// 아래
		{{s, -s, s}, {0, -1, 0}, {0, 0}}, {{-s, -s, s}, {0, -1, 0}, {1, 0}},
		{{s, -s, -s}, {0, -1, 0}, {0, 1}}, {{-s, -s, -s}, {0, -1, 0}, {1, 1}},

		// 오른쪽
		{{s, s, -s}, {1, 0, 0}, {0, 0}}, {{s, s, s}, {1, 0, 0}, {1, 0}},
		{{s, -s, -s}, {1, 0, 0}, {0, 1}}, {{s, -s, s}, {1, 0, 0}, {1, 1}},

		// 왼쪽
		{{-s, s, s}, {-1, 0, 0}, {0, 0}}, {{-s, s, -s}, {-1, 0, 0}, {1, 0}},
		{{-s, -s, s}, {-1, 0, 0}, {0, 1}}, {{-s, -s, -s}, {-1, 0, 0}, {1, 1}},
	};

	data.indices = {
		// 앞
		 0,1,2 , 2, 1, 3,
		 // 뒤
		 4, 5, 6, 6, 5, 7,
		 //위
		 8, 9, 10, 10, 9, 11,
		 //아래
		 12, 13, 14, 14, 13, 15,
		 //오른
		16, 17, 18, 18, 17, 19,
		//왽쪽
		20, 21, 22, 22, 21, 23
	};

	return data;
}

MeshData GeometryFactory::CreateSphere(float radius, UINT sliceCount, UINT stackCount)
{
	return MeshData();
}
