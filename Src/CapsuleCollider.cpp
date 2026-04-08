#include "CapsuleCollider.h"

void CapsuleCollider::GetSegment(Vector3& outTop, Vector3& outBottom)
{
	// 1. 현재 오브젝트의 월드 위치와 회전 정보를 가져옵니다.
	Vector3 center = GetCenter();

	// 2. 캡슐의 방향 (일반적으로 로컬 Y축을 사용)
	Vector3 up = { 0, 1,0 };

	// 3. 선분은 중심에서 위아래로 (전체 높이 /2)만큼 떨어진 지점
	// 실제 캡슐 높이는 원통 높이 + 2 * 반지름
	float halfHeight = m_height * 0.5f;

	outTop = center + (up * halfHeight);
	outBottom = center - (up * halfHeight);

}


