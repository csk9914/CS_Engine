#pragma once
#include "Collider.h"

class SphereCollider : public Collider
{
public:
    SphereCollider() : Collider(ColliderType::Sphere) {}

    void SetRadius(float radius) { m_radius = radius; }
    float GetRadius() const { return m_radius; }

private:
    float m_radius = 1.0f;
};