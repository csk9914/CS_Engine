#include "CollisionManager.h"
#include "GameObject.h"
#include "Collider.h"
#include "CollisionHelper.h"
#include "Rigidbody.h"
#include <iostream>

void CollisionManager::Init()
{
	// 일단 모든 칸을 nullptr로 초기화
	for (int i = 0; i < (int)ColliderType::Size; ++i)
		for (int j = 0; j < (int)ColliderType::Size; ++j)
			m_matrix[i][j] = nullptr;

	// sphere vs shpere
	m_matrix[(int)ColliderType::Sphere][(int)ColliderType::Sphere] = &CollisionHelper::SphereVsSphere;

	//box vs box
	m_matrix[(int)ColliderType::Box][(int)ColliderType::Box] = &CollisionHelper::BoxVsBox;

	// capsule vs capsule
	m_matrix[(int)ColliderType::Capsule][(int)ColliderType::Capsule] = &CollisionHelper::CapsuleVsCapsule;

	// sphere vs box
	m_matrix[(int)ColliderType::Sphere][(int)ColliderType::Box] = &CollisionHelper::SphereVsBox;

	// box vs sphere
	m_matrix[(int)ColliderType::Box][(int)ColliderType::Sphere] = [](Collider* a, Collider* b)
		{
			return CollisionHelper::SphereVsBox(b, a);
		};

	// capsule vs sphere
	m_matrix[(int)ColliderType::Capsule][(int)ColliderType::Sphere] = &CollisionHelper::CapsuleVsSphere;

	// sphere vs capsule
	m_matrix[(int)ColliderType::Sphere][(int)ColliderType::Capsule] = [](Collider* a, Collider* b)
		{
			return CollisionHelper::CapsuleVsSphere(b, a);
		};

	// capsule vs box
	m_matrix[(int)ColliderType::Capsule][(int)ColliderType::Box] = &CollisionHelper::CapsuleVsBox;

	// box vs capsule
	m_matrix[(int)ColliderType::Box][(int)ColliderType::Capsule] = [](Collider* a, Collider* b)
		{
			return CollisionHelper::CapsuleVsBox(b, a);
		};
}

void CollisionManager::Update()
{
	// 1. 삭제 대기열 처리 
	if (!m_pendingDelete.empty())
	{
		for (auto* col : m_pendingDelete)
		{
			m_colliders.erase(std::remove(m_colliders.begin(), m_colliders.end(), col), m_colliders.end());
		}
		m_pendingDelete.clear();
	}

	for (auto iterA = m_colliders.begin(); iterA != m_colliders.end(); iterA++)
	{
		Collider* colA = *iterA;
		if (!colA->GetActive()) continue;

		for (auto iterB = std::next(iterA); iterB != m_colliders.end(); iterB++)
		{
			Collider* colB = *iterB;
			if (!colB->GetActive()) continue;

			int typeA = (int)colA->GetColliderType();
			int typeB = (int)colB->GetColliderType();

			// 행렬에서 함수 꺼내기
			CollisionCheck func = m_matrix[typeA][typeB];

			ResolveCollision(func(colA, colB));



		}
	}

}

void CollisionManager::ResolveCollision(const CollisionInfo& info)
{
	if (!info.isColliding) return;

	GameObject* objA = info.mine->gameObject();
	Rigidbody* rbA = objA->GetComponent<Rigidbody>();

	GameObject* objB = info.other->gameObject();
	Rigidbody* rbB = objB->GetComponent<Rigidbody>();

	// 두 물체의 이동 가능 여부 판단
	bool canMoveA = (rbA && !rbA->IsKinematic());
	bool canMoveB = (rbB && !rbB->IsKinematic());

	if (!canMoveA && !canMoveB) return;

	// --- 1. 위치 보정 (Position Correction) ---
	// 겹친 만큼 즉시 밀어내어 충돌 상태를 해소합니다.
	if (canMoveA && !canMoveB)
	{
		CorrectOverlap(objA, info.normal * -info.penetration);
	}
	else if (!canMoveA && canMoveB)
	{
		CorrectOverlap(objB, info.normal * info.penetration);
	}
	else if (canMoveA && canMoveB)
	{
		// 둘 다 움직일 수 있다면 절반씩 밀어냅니다.
		CorrectOverlap(objA, info.normal * -info.penetration * 0.5f);
		CorrectOverlap(objB, info.normal * info.penetration * 0.5f);
	}

	// --- 2. 속도 보정 (Velocity Resolution) ---
	// 위치를 옮겨도 속도가 그대로면 다음 프레임에 다시 충돌체 안으로 파고듭니다.
	// 충돌 법선(Normal) 방향의 속도 성분을 제거해야 합니다.

	if (canMoveA)
	{
		Vector3 velA = rbA->GetVelocity();
		// info.normal이 A에서 B를 향한다고 가정할 때, 
		// A 입장에서 법선 방향으로 진행 중(Dot > 0)이라면 그 성분만큼 빼줍니다.
		float dot = Vector3::Dot(velA, info.normal);
		if (dot > 0.0f)
		{
			rbA->SetVelocity(velA - info.normal * dot);
		}
	}

	if (canMoveB)
	{
		Vector3 velB = rbB->GetVelocity();
		// B 입장에서는 법선 반대 방향으로 진행 중(Dot < 0)일 때 충돌 중인 것입니다.
		float dot = Vector3::Dot(velB, info.normal);
		if (dot < 0.0f)
		{
			rbB->SetVelocity(velB - info.normal * dot);
		}
	}
}

void CollisionManager::CorrectOverlap(GameObject* a, Vector3 force)
{
	a->GetTransform()->AddPosition(force);
}


// 콜라이더가 Awake될 때 등록, OnDestroy될 때 해제
void CollisionManager::AddCollider(Collider* col)
{
	m_colliders.push_back(col);
}


void CollisionManager::RemoveCollider(Collider* col)
{
	// 즉시 지우지 않고 대기열에 추가
	m_pendingDelete.push_back(col);
}
