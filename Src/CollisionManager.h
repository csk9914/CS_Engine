#pragma once
#include <vector>
#include <iostream>

#include "Collider.h"

// 함수 포인터 타입 정의 (Collider 2개를 받아서 bool 반환)
typedef bool (*CollisionCheck)(Collider*, Collider*);

class CollisionManager
{
public:

    static CollisionManager* Instance()
    {
        static CollisionManager instance;
        return &instance;
    }

    void Init();
    void Update();

public:
    // 콜라이더가 Awake될 때 등록, OnDestroy될 때 해제
    void AddCollider(Collider* col) { m_colliders.push_back(col); }
    void RemoveCollider(Collider* col);

private:
    std::vector<Collider*> m_colliders;

    // 충돌 행렬: [타입A][타입B]
    CollisionCheck m_matrix[(int)ColliderType::Size][(int)ColliderType::Size];

};