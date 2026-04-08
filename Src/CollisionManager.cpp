#include "CollisionManager.h"
#include "GameObject.h"
#include "Collider.h"
#include "CollisionHelper.h"

void CollisionManager::Init()
{
    // 일단 모든 칸을 nullptr로 초기화
    for (int i = 0; i < (int)ColliderType::Size; ++i)
        for (int j = 0; j < (int)ColliderType::Size; ++j)
            m_matrix[i][j] = nullptr;

    // sphere vs shpere
    m_matrix[(int)ColliderType::Sphere][(int)ColliderType::Sphere] = &CollisionHelper::SphereVsSphere;

    // box vs box
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
    for (size_t i = 0; i < m_colliders.size(); ++i)
    {
        for (size_t j = i + 1; j < m_colliders.size(); ++j)
        {
            Collider* colA = m_colliders[i];
            Collider* colB = m_colliders[j];

            int typeA = (int)colA->GetColliderType();
            int typeB = (int)colB->GetColliderType();

            // 행렬에서 함수 꺼내기
            CollisionCheck func = m_matrix[typeA][typeB];

            if (func != nullptr && func(colA, colB))
            {
                // 콘솔창에 이름 출력!
                std::cout << "BOOM! " << colA->GetOwner()->GetName()
                    << " hit " << colB->GetOwner()->GetName() << std::endl;
            }
        }
    }
}