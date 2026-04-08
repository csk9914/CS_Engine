#include "CollisionHelper.h"
#include "GameObject.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"

bool CollisionHelper::SphereVsSphere(Collider* a, Collider* b)
{
    if (b->GetColliderType() == ColliderType::Sphere)
    {
        SphereCollider* colA = static_cast<SphereCollider*>(a);
        SphereCollider* colB = static_cast<SphereCollider*>(b);

        // 부모의 스케일 중 가장 큰 값을 반지름에 곱해줍니다.
        float ranA = colA->GetRadius() * colA->GetOwner()->GetTransform()->GetScale().MaxElement();
        float ranB = colB->GetRadius() * colB->GetOwner()->GetTransform()->GetScale().MaxElement();

        // 중심점 간의 거리 계산
        float disSq = Vector3::DistanceSquared(colA->GetCenter(), colB->GetCenter());

        // 거리 < 반지름의 합 이면 충돌
        float sumRad = ranA + ranB;
        return disSq <= (sumRad * sumRad);
    }
    return false;
}

bool CollisionHelper::BoxVsBox(Collider* a, Collider* b)
{
	if (b->GetColliderType() == ColliderType::Box)
	{
		BoxCollider* colA = static_cast<BoxCollider*>(a);
		BoxCollider* colB = static_cast<BoxCollider*>(b);

		Vector3 minA = colA->GetMin();
		Vector3 maxA = colA->GetMax();
		Vector3 minB = colB->GetMin();
		Vector3 maxB = colB->GetMax();

		// X, Y, Z 모든 축에서 겹쳐야 충돌이다! (하나라도 안 겹치면 false)
		return (minA.x <= maxB.x && maxA.x >= minB.x) &&
			(minA.y <= maxB.y && maxA.y >= minB.y) &&
			(minA.z <= maxB.z && maxA.z >= minB.z);
	}
	return false;
}

bool CollisionHelper::CapsuleVsCapsule(Collider* a, Collider* b)
{
    CapsuleCollider* capA = static_cast<CapsuleCollider*>(a);
    CapsuleCollider* capB = static_cast<CapsuleCollider*>(b);

    Vector3 posA1, posA2, posB1, posB2;
    capA->GetSegment(posA1, posA2);
    capB->GetSegment(posB1, posB2);

    // 두 선분 사이의 최단 거리를 구하는 복잡한 공식 생략 (대략적인 원리)
    // 선분 A 위의 점 P1, 선분 B 위의 점 P2 사이의 거리 d의 최솟값 계산
    float distSq = CalculateClosestPointsDistSq(posA1, posA2, posB1, posB2);

    float radiusSum = capA->GetRadius() + capB->GetRadius();
    return distSq <= (radiusSum * radiusSum);
}

bool CollisionHelper::SphereVsBox(Collider* a, Collider* b)
{
    if (b->GetColliderType() == ColliderType::Box)
    {
        SphereCollider* sphere = static_cast<SphereCollider*>(a);
        BoxCollider* box = static_cast<BoxCollider*>(b);

        Vector3 sphereCenter = sphere->GetCenter();
        Vector3 boxMin = box->GetMin();
        Vector3 boxMax = box->GetMax();

        // 상자 내에서 구 중심과 가장 가까운 점(Closest Point) 찾기
        float closestX = std::max(boxMin.x, std::min(sphereCenter.x, boxMax.x));
        float closestY = std::max(boxMin.y, std::min(sphereCenter.y, boxMax.y));
        float closestZ = std::max(boxMin.z, std::min(sphereCenter.z, boxMax.z));

        float distSq = Vector3::DistanceSquared(sphereCenter, { closestX, closestY, closestZ });
        float radius = sphere->GetRadius() * sphere->GetOwner()->GetTransform()->GetScale().MaxElement();

        return distSq <= (radius * radius);
    }

    return false;
}

bool CollisionHelper::CapsuleVsSphere(Collider* a, Collider* b)
{
    CapsuleCollider* capsule = static_cast<CapsuleCollider*>(a);
    SphereCollider* sphere = static_cast<SphereCollider*>(b);

    // 1. 캡슐의 선분 양 끝점(A, B) 가져오기
    Vector3 A, B;
    capsule->GetSegment(A, B);

    // 2. 구의 중심점 C
    Vector3 C = sphere->GetCenter();

    // 3. 선분 AB 위에서 점 C와 가장 가까운 점 P 찾기 (선분 투영)
    Vector3 ab = B - A;
    // t = dot(CA, AB) / dot(AB, AB)
    float t = Vector3::Dot(C - A, ab) / Vector3::DistanceSquared(A, B);
    t = std::max(0.0f, std::min(1.0f, t)); // 선분 범위로 클램핑

    Vector3 P = A + ab * t;

    // 4. 점 P와 구 중심 C 사이의 거리 비교
    float distSq = Vector3::DistanceSquared(P, C);
    float radiusSum = capsule->GetRadius() + (sphere->GetRadius() * sphere->GetOwner()->GetTransform()->GetScale().MaxElement());

    return distSq <= (radiusSum * radiusSum);
}

bool CollisionHelper::CapsuleVsBox(Collider* a, Collider* b)
{
    CapsuleCollider* capsule = static_cast<CapsuleCollider*>(a);
    BoxCollider* box = static_cast<BoxCollider*>(b);

    Vector3 p1, p2;
    capsule->GetSegment(p1, p2);

    Vector3 bMin = box->GetMin();
    Vector3 bMax = box->GetMax();

    // 1. 선분(p1-p2)과 박스(AABB) 사이의 최단 거리 제곱을 구함
    float distSq = SqrDistSegmentAABB(p1, p2, bMin, bMax);

    // 2. 캡슐 반지름의 제곱과 비교
    float r = capsule->GetRadius();
    return distSq <= (r * r);
}

float CollisionHelper::CalculateClosestPointsDistSq(Vector3 p1, Vector3 p2, Vector3 q1, Vector3 q2)
{
    Vector3 d1 = p2 - p1; // 선분 1의 방향 벡터
    Vector3 d2 = q2 - q1; // 선분 2의 방향 벡터
    Vector3 r = p1 - q1;

    float a = Vector3::Dot(d1, d1); // 선분 1의 길이 제곱
    float e = Vector3::Dot(d2, d2); // 선분 2의 길이 제곱
    float f = Vector3::Dot(d2, r);

    float s, t;

    // 두 선분이 평행하거나 한 점일 경우 처리
    if (a <= 1e-6f && e <= 1e-6f) return Vector3::DistanceSquared(p1, q1);
    if (a <= 1e-6f)
    {
        s = 0.0f;
        t = std::clamp(f / e, 0.0f, 1.0f);
    }
    else
    {
        float c = Vector3::Dot(d1, r);
        if (e <= 1e-6f)
        {
            t = 0.0f;
            s = std::clamp(-c / a, 0.0f, 1.0f);
        }
        else
        {
            // 일반적인 경우: 두 선분 사이의 최단 매개변수 s, t 계산
            float b = Vector3::Dot(d1, d2);
            float denom = a * e - b * b;

            if (denom != 0.0f)
            {
                s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }
            else
            {
                s = 0.0f; // 평행할 경우 아무데나
            }

            t = (b * s + f) / e;

            if (t < 0.0f)
            {
                t = 0.0f;
                s = std::clamp(-c / a, 0.0f, 1.0f);
            }
            else if (t > 1.0f)
            {
                t = 1.0f;
                s = std::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    Vector3 closestL1 = p1 + d1 * s;
    Vector3 closestL2 = q1 + d2 * t;

    return Vector3::DistanceSquared(closestL1, closestL2);
}

float CollisionHelper::SqrDistSegmentAABB(Vector3 p1, Vector3 p2, Vector3 bMin, Vector3 bMax)
{
    // 선분의 시작점 p1이 박스 내부에 있는지 확인하기 위해 박스에 클램핑한 점 찾기
    auto getClosestPoint = [&](Vector3 p)
        {
            return Vector3(
                std::max(bMin.x, std::min(p.x, bMax.x)),
                std::max(bMin.y, std::min(p.y, bMax.y)),
                std::max(bMin.z, std::min(p.z, bMax.z))
            );
        };

    // 1. 선분의 양 끝점이 박스 안에 있다면 즉시 충돌(거리 0)
    // (정밀도를 위해 선분을 쪼개거나 분리축 이론(SAT)을 쓰는 게 좋지만, 
    // 가성비를 위해 끝점과 중간점을 체크합니다.)

    // 2. 실제로는 선분을 매개변수 t로 표현하고 
    // 박스의 6개 평면에 대해 선분-평면 교차 및 거리 검사를 수행해야 합니다.
    // (코드가 매우 길어지므로, 엔진 수준에서는 보통 SAT 알고리즘을 사용합니다.)

    // 임시 가성비 로직: 선분 위의 여러 지점에서 박스까지의 거리 중 최솟값
    float minDistSq = FLT_MAX;
    const int samples = 4; // 선분을 4등분해서 샘플링 (정밀도 조절 가능)
    for (int i = 0; i <= samples; ++i)
    {
        float t = (float)i / samples;
        Vector3 p = p1 + (p2 - p1) * t;
        Vector3 closestInBox = getClosestPoint(p);
        minDistSq = std::min(minDistSq, Vector3::DistanceSquared(p, closestInBox));
    }

    return minDistSq;
}
