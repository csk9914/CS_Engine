#pragma once
#include "Collider.h"

class SphereCollider : public Collider
{
public:
    SphereCollider(const std::string& name = "SphereCollider") : Collider(ColliderType::Sphere, name) {}

    void SetRadius(float radius) { m_radius = radius; }
    float GetRadius() const { return m_radius; }

private:
    float m_radius = 1.0f;
};