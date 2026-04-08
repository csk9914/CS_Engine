#pragma once
#include "Vector3.h"
#include "Collider.h"

class BoxCollider : public Collider
{
public:
    BoxCollider(const std::string& name = "BoxCollider") : Collider(ColliderType::Box, name) {}

    void SetSize(const Vector3& size) { m_size = size; }
    Vector3 GetSize() const { return m_size; }

    // AABB를 위한 최소/최대 지점 계산
    Vector3 GetMin();
    Vector3 GetMax();

private:
    Vector3 m_size = { 1.0f, 1.0f, 1.0f };
};