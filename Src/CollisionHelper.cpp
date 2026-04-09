#include "CollisionHelper.h"
#include "GameObject.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include <algorithm>
#include <math.h>
#include <limits>

CollisionInfo CollisionHelper::CapsuleVsCapsule(Collider* a, Collider* b)
{
    CollisionInfo info;
    CapsuleCollider* capA = static_cast<CapsuleCollider*>(a);
    CapsuleCollider* capB = static_cast<CapsuleCollider*>(b);

    Vector3 a1, a2, b1, b2;
    capA->GetSegment(a1, a2);
    capB->GetSegment(b1, b2);

    // 두 선분 사이의 최단 거리 점(P1, P2)을 찾는 헬퍼 함수 호출
    Vector3 p1, p2;
    float distance = sqrtf(ClosestPointsBetweenSegments(a1, a2, b1, b2, p1, p2));

    float radiusSum = capA->GetRadius() + capB->GetRadius();

    if (distance <= radiusSum)
    {
        info.isColliding = true;
        info.mine = a;
        info.other = b;

        // p1(A의 최단점)에서 p2(B의 최단점)로 향하는 방향이 법선
        info.normal = (distance > 0.0001f) ? (p2 - p1).Normalized() : Vector3(0, 1, 0);
        info.penetration = radiusSum - distance;
    }
    return info;
}

CollisionInfo CollisionHelper::SphereVsSphere(Collider* a, Collider* b)
{
    CollisionInfo info;
    SphereCollider* colA = static_cast<SphereCollider*>(a);
    SphereCollider* colB = static_cast<SphereCollider*>(b);

    float rA = colA->GetRadius() * colA->GetGameObject()->GetTransform()->GetScale().MaxElement();
    float rB = colB->GetRadius() * colB->GetGameObject()->GetTransform()->GetScale().MaxElement();

    Vector3 posA = colA->GetCenter();
    Vector3 posB = colB->GetCenter();

    Vector3 dir = posB - posA;
    float distance = dir.Length(); // 제곱 대신 실제 길이 사용
    float radiusSum = rA + rB;

    if (distance <= radiusSum)
    {
        info.isColliding = true;
        info.mine = a;
        info.other = b;
        // 거리가 0일 경우(완전히 겹침)를 대비해 안전처리
        info.normal = (distance > 0.0001f) ? dir.Normalized() : Vector3(0, 1, 0);
        info.penetration = radiusSum - distance;
    }
    return info;
}

CollisionInfo CollisionHelper::CapsuleVsSphere(Collider* a, Collider* b)
{
    CollisionInfo info;
    CapsuleCollider* capsule = static_cast<CapsuleCollider*>(a);
    SphereCollider* sphere = static_cast<SphereCollider*>(b);

    Vector3 A, B, C = sphere->GetCenter();
    capsule->GetSegment(A, B);

    Vector3 ab = B - A;
    float t = Vector3::Dot(C - A, ab) / ab.LengthSqrt(); // 이건 공식상 어쩔 수 없이 제곱이 들어감(분모)
    t = std::clamp(t, 0.0f, 1.0f);
    Vector3 P = A + ab * t;

    Vector3 diff = C - P;
    float distance = diff.Length(); // 실제 거리 사용
    float rSum = capsule->GetRadius() + sphere->GetRadius();

    if (distance <= rSum)
    {
        info.isColliding = true;
        info.mine = a;
        info.other = b;
        info.normal = (distance > 0.0001f) ? diff.Normalized() : Vector3(0, 1, 0);
        info.penetration = rSum - distance;
    }
    return info;
}

// 1. Box vs Box (가장 적게 겹친 축 기준 밀어내기)
CollisionInfo CollisionHelper::BoxVsBox(Collider* a, Collider* b)
{
    CollisionInfo info;
    BoxCollider* colA = static_cast<BoxCollider*>(a);
    BoxCollider* colB = static_cast<BoxCollider*>(b);

    Vector3 minA = colA->GetMin();
    Vector3 maxA = colA->GetMax();
    Vector3 minB = colB->GetMin();
    Vector3 maxB = colB->GetMax();

    float xOverlap = std::min(maxA.x, maxB.x) - std::max(minA.x, minB.x);
    float yOverlap = std::min(maxA.y, maxB.y) - std::max(minA.y, minB.y);
    float zOverlap = std::min(maxA.z, maxB.z) - std::max(minA.z, minB.z);

    if (xOverlap <= 0 || yOverlap <= 0 || zOverlap <= 0) return info;

    info.isColliding = true;
    info.mine = a;
    info.other = b;

    // 가장 적게 겹친 축 방향으로 밀어내기
    if (xOverlap < yOverlap && xOverlap < zOverlap)
    {
        info.penetration = xOverlap;
        info.normal = (colA->GetCenter().x < colB->GetCenter().x) ? Vector3(-1, 0, 0) : Vector3(1, 0, 0);
    }
    else if (yOverlap < zOverlap)
    {
        info.penetration = yOverlap;
        info.normal = (colA->GetCenter().y < colB->GetCenter().y) ? Vector3(0, -1, 0) : Vector3(0, 1, 0);
    }
    else
    {
        info.penetration = zOverlap;
        info.normal = (colA->GetCenter().z < colB->GetCenter().z) ? Vector3(0, 0, -1) : Vector3(0, 0, 1);
    }
    return info;
}

// 2. Sphere vs Box (Closest Point 방식)
CollisionInfo CollisionHelper::SphereVsBox(Collider* a, Collider* b)
{
    CollisionInfo info;
    SphereCollider* sphere = static_cast<SphereCollider*>(a);
    BoxCollider* box = static_cast<BoxCollider*>(b);

    Vector3 center = sphere->GetCenter();
    Vector3 bMin = box->GetMin();
    Vector3 bMax = box->GetMax();

    // 박스 내에서 구의 중심과 가장 가까운 점 찾기
    Vector3 closest(
        std::clamp(center.x, bMin.x, bMax.x),
        std::clamp(center.y, bMin.y, bMax.y),
        std::clamp(center.z, bMin.z, bMax.z)
    );

    Vector3 diff = center - closest;
    float distance = diff.Length();
    float r = sphere->GetRadius() * sphere->GetGameObject()->GetTransform()->GetScale().MaxElement();

    if (distance <= r)
    {
        info.isColliding = true;
        info.mine = a;
        info.other = b;
        // 구 중심이 박스 안에 쏙 들어간 경우(distance 0) 예외 처리
        if (distance < 0.0001f)
        {
            info.normal = Vector3(0, 1, 0);
            info.penetration = r;
        }
        else
        {
            info.normal = diff.Normalized();
            info.penetration = r - distance;
        }
    }
    return info;
}

// 3. Capsule vs Box (샘플링 기반)
CollisionInfo CollisionHelper::CapsuleVsBox(Collider* a, Collider* b)
{
    CollisionInfo info;
    CapsuleCollider* capsule = static_cast<CapsuleCollider*>(a);
    BoxCollider* box = static_cast<BoxCollider*>(b);

    Vector3 p1, p2;
    capsule->GetSegment(p1, p2);

    // 우리가 만든 DistSegmentAABB 활용
    float distance = DistSegmentAABB(p1, p2, box->GetMin(), box->GetMax());
    float r = capsule->GetRadius();

    if (distance <= r)
    {
        info.isColliding = true;
        info.mine = a;
        info.other = b;

        // 방향은 대략적으로 캡슐 중심에서 박스 중심 방향의 반대로 설정
        info.normal = (capsule->GetCenter() - box->GetCenter()).Normalized();
        info.penetration = r - distance;
    }
    return info;
}

float CollisionHelper::DistSegmentAABB(Vector3 p1, Vector3 p2, Vector3 bMin, Vector3 bMax)
{
    auto getClosestPoint = [&](Vector3 p)
        {
            return Vector3(
                std::clamp(p.x, bMin.x, bMax.x), // max -> bMax로 수정
                std::clamp(p.y, bMin.y, bMax.y),
                std::clamp(p.z, bMin.z, bMax.z)
            );
        };

    float minDistance = FLT_MAX;
    const int samples = 5; // 등분해서 체크
    for (int i = 0; i <= samples; ++i)
    {
        float t = (float)i / samples;
        Vector3 p = p1 + (p2 - p1) * t;
        Vector3 closestInBox = getClosestPoint(p);

        // DistanceSquared 대신 Distance 사용
        minDistance = std::min(minDistance, (closestInBox - p).Length());
    }
    return minDistance;
}

float CollisionHelper::ClosestPointsBetweenSegments(Vector3 p1, Vector3 p2, Vector3 q1, Vector3 q2, Vector3& outP1, Vector3& outP2)
{
    Vector3 d1 = p2 - p1;
    Vector3 d2 = q2 - q1;
    Vector3 r = p1 - q1;

    float a = Vector3::Dot(d1, d1);
    float e = Vector3::Dot(d2, d2);
    float f = Vector3::Dot(d2, r);
    float s, t;

    float b = Vector3::Dot(d1, d2);
    float c = Vector3::Dot(d1, r);
    float denom = a * e - b * b;

    if (denom != 0.0f) s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
    else s = 0.0f;

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

    outP1 = p1 + d1 * s;
    outP2 = q1 + d2 * t;

    return (outP1 - outP2).LengthSqrt(); // 여기서는 공식 내부적으로 제곱을 반환 (나중에 루트 씌움)
}