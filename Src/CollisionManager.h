#pragma once
#include <vector>
#include <iostream>

#include "Collider.h"

/*
    유니티에서 충돌 조건.....
    1. 두 오브젝트 모두 Collider를 가지고 있어야 함
    2. 최소한 한 쪽은 isTrigger가 활성돼 있어야 함
    3. 최소한 한 쪽은 Rigidbody를 가지고 있어야 함
*/

// 함수 포인터 타입 정의 (Collider 2개를 받아서 bool 반환)
typedef CollisionInfo(*CollisionCheck)(Collider*, Collider*);

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
    void ResolveCollision(const CollisionInfo& info);
    void CorrectOverlap(GameObject* a, Vector3 force);

public:
    // 콜라이더가 Awake될 때 등록, OnDestroy될 때 해제
    void AddCollider(Collider* col);
    void RemoveCollider(Collider* col);

private:
    std::vector<Collider*> m_colliders;
    std::vector<Collider*> m_pendingDelete; // 삭제 대기열

    // 충돌 행렬: [타입A][타입B]
    CollisionCheck m_matrix[(int)ColliderType::Size][(int)ColliderType::Size] = {};

};