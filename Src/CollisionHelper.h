#pragma once

#include "Vector3.h"

class Collider;
class SphereCollider;
class BoxCollider;
class CapsuleCollider;
struct CollisionInfo;

class CollisionHelper
{
public:
	static CollisionInfo SphereVsSphere(Collider* a, Collider* b);
	static CollisionInfo BoxVsBox(Collider* a, Collider* b);
	static CollisionInfo CapsuleVsCapsule(Collider* a, Collider* b);

public:
	static CollisionInfo SphereVsBox(Collider* a, Collider* b);
	static CollisionInfo CapsuleVsSphere(Collider* a, Collider* b);
	static CollisionInfo CapsuleVsBox(Collider* a, Collider* b);

	static float DistSegmentAABB(Vector3 p1, Vector3 p2, Vector3 bMin, Vector3 bMax);
	static float ClosestPointsBetweenSegments(Vector3 p1, Vector3 p2, Vector3 q1, Vector3 q2, Vector3& outP1, Vector3& outP2);

};