#pragma once

#include "Vector3.h"

class Collider;
class SphereCollider;
class BoxCollider;
class CapsuleCollider;

class CollisionHelper
{
public:
	static bool SphereVsSphere(Collider* a, Collider* b);
	static bool BoxVsBox(Collider* a, Collider* b);
	static bool CapsuleVsCapsule(Collider* a, Collider* b);

public:
	static bool SphereVsBox(Collider* a, Collider* b);
	static bool CapsuleVsSphere(Collider* a, Collider* b);
	static bool CapsuleVsBox(Collider* a, Collider* b);
public:
	static float CalculateClosestPointsDistSq(Vector3 p1, Vector3 p2, Vector3 q1, Vector3 q2);
	static float CollisionHelper::SqrDistSegmentAABB(Vector3 p1, Vector3 p2, Vector3 bMin, Vector3 bMax);
};