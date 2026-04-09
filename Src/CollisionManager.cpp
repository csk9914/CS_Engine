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



	GameObject* objA = info.mine->GetGameObject();
	Rigidbody* rbA = objA->GetComponent<Rigidbody>();

	GameObject* objB = info.other->GetGameObject();
	Rigidbody* rbB = objB->GetComponent<Rigidbody>();

	//콘솔창에 이름 출력!
	std::cout << "BOOM! " << objA->GetName() << " hit " << objB->GetName() << std::endl;


	// 둘 다 Rigidbody가 없거나, 둘 다 IsKinematic이면 밀어내지 않음
	bool canMoveA = (rbA && !rbA->IsKinematic());
	bool canMoveB = (rbB && !rbB->IsKinematic());

	if (!canMoveA && !canMoveB) return;

	// 1. 위치 보정 (Position Correction)
	// 겹친 깊이(penetration)만큼 법선 방향(normal)으로 밀어냄
	if (canMoveA && !canMoveB) // A만 물리 적용 중
	{
		CorrectOverlap(objA, info.normal * -info.penetration);
	}
	else if (!canMoveA && canMoveB) //B만 물리 적용
	{
		CorrectOverlap(objB, info.normal * info.penetration);
	}
	else if (canMoveA && canMoveB)	// 둘 다 적용
	{
		CorrectOverlap(objA, info.normal * -info.penetration * 0.5f);
		CorrectOverlap(objB, info.normal * info.penetration * 0.5f);
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
