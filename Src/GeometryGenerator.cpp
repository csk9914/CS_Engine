#include "GeometryGenerator.h"
#include "DX11Renderer.h"

#include <vector>        
#include <cstdint>
#include <cmath>           
#include <DirectXMath.h>

using namespace DirectX;

MeshData GeometryGenerator::CreateRectangle(float width, float height)
{

	return MeshData();
}

MeshData GeometryGenerator::CreatePlaneData(float size)
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

MeshData GeometryGenerator::CreateCubeData(float size)
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

MeshData GeometryGenerator::CreateSphereData(float radius, UINT sliceCount, UINT stackCount)
{

	MeshData data;

	// 1. 정점(Vertex) 생성
	// 북극(top)부터 남극(bottom)까지 루프
	for (std::uint32_t i = 0; i <= stackCount; i++)
	{
		float phi = i * XM_PI / stackCount;

		// 각 층(stack)마다 한 바퀴(slice) 돌며 정점 생성
		for (std::uint32_t j = 0; j <= sliceCount; j++)
		{
			float theta = j * 2.0f * XM_PI / sliceCount;

			Vertex v;

			// 구의 좌표 공식
			// x = r * sin(phi) * cos(theta)
			v.position.x = radius * sinf(phi) * cosf(theta);

			// y = r * cos(phi)
			v.position.y = radius * cosf(phi);

			// z = r * sin(phi) * sin(theta)
			v.position.z = radius * sinf(phi) * sinf(theta);

			// 법선(Normal)은 중심에서 바깥으로 나가는 방향 (구에서는 좌표/반지름과 같음)
			XMVECTOR nor = XMLoadFloat3(&v.position);
			XMStoreFloat3(&v.normal, XMVector3Normalize(nor));

			// UV 좌표 (0~1 범위로 매핑)
			v.uv.x = (float)j / sliceCount;
			v.uv.y = (float)i / stackCount;

			data.vertices.push_back(v);
		}
	}

	// 2. 인덱스(Index) 생성 (사각형망을 삼각형으로 쪼개기)
	for (std::uint32_t i = 0; i < stackCount; ++i)
	{
		for (std::uint32_t j = 0; j < sliceCount; ++j)
		{
			// 현재 사각형의 네 귀퉁이 인덱스 계산
			std::uint32_t first = i * (sliceCount + 1) + j;
			std::uint32_t second = first + sliceCount + 1;

			// 삼각형 1
			data.indices.push_back(first);
			data.indices.push_back(first + 1);
			data.indices.push_back(second);

			// 삼각형 2
			data.indices.push_back(second);
			data.indices.push_back(first + 1);
			data.indices.push_back(second + 1);
		}
	}

	return data;
}

MeshData GeometryGenerator::CreateCapsuleData(float radius, float height, UINT sliceCount, UINT stackCount)
{
	MeshData data;

	// 스택(위아래 분할)은 반구 두 개를 합쳐야 하므로 짝수가 좋습니다.
	float phiStep = XM_PI / stackCount;
	float thetaStep = 2.0f * XM_PI / sliceCount;

	// 1. 정점(Vertices) 생성
	for (uint32_t i = 0; i <= stackCount; ++i)
	{
		float phi = i * phiStep;

		for (uint32_t j = 0; j <= sliceCount; ++j)
		{
			float theta = j * thetaStep;

			Vertex v;

			// 구의 기본 좌표계 계산
			float x = radius * sinf(phi) * cosf(theta);
			float y = radius * cosf(phi);
			float z = radius * sinf(phi) * sinf(theta);

			v.position = XMFLOAT3(x, y, z);

			// 캡슐의 핵심: Y축 오프셋 적용
			// 상단 반구 구간 (위쪽)
			if (phi <= XM_PI * 0.5f + 0.001f)
				v.position.y += height * 0.5f;
			// 하단 반구 구간 (아래쪽)
			else
				v.position.y -= height * 0.5f;

			v.normal = v.position; // 법선 벡터 계산 (오프셋 제거 전 방향 필요시 로직 수정 가능)
			// 실제 법선은 오프셋을 뺀 순수 구의 형태에서 계산하는 것이 더 정확합니다.
			XMStoreFloat3(&v.normal, XMVector3Normalize(XMLoadFloat3(&v.normal)));

			v.uv = XMFLOAT2((float)j / sliceCount, (float)i / stackCount);

			data.vertices.push_back(v);
		}
	}

	// 2. 인덱스(Indices) 생성 (기본 구/원통 방식과 동일)
	for (uint32_t i = 0; i < stackCount; ++i)
	{
		for (uint32_t j = 0; j < sliceCount; ++j)
		{
			data.indices.push_back(i * (sliceCount + 1) + j);
			data.indices.push_back((i + 1) * (sliceCount + 1) + j);
			data.indices.push_back((i + 1) * (sliceCount + 1) + (j + 1));

			data.indices.push_back(i * (sliceCount + 1) + j);
			data.indices.push_back((i + 1) * (sliceCount + 1) + (j + 1));
			data.indices.push_back(i * (sliceCount + 1) + (j + 1));
		}
	}

	return data;
}
