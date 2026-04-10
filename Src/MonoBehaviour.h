#pragma once
#include "Component.h"
#include "Transform.h"
#include "GameObject.h"
#include "Rigidbody.h"
#include "MeshRenderer.h"
#include "MeshFilter.h"

#include <string>

class MonoBehaviour : public Component
{
public:
    MonoBehaviour(std::string name = "MonoBehaviour") : Component(name) {}
    virtual ~MonoBehaviour() {}

    virtual void Awake() {}
    virtual void Start() {}
    virtual void FixedUpdate(float deltaTime) {}
    virtual void Update(float deltaTime) {}

    virtual void OnEnable() override {}
    virtual void OnDisable() override {}
};