#pragma once
#include "Component.h"
#include <string>

class MonoBehaviour : public Component
{
public:
    MonoBehaviour(std::string name = "MonoBehaviour") : Component(name) {}
    virtual ~MonoBehaviour() {}

    virtual void Awake() {}
    virtual void Start() {}
    virtual void Update(float deltaTime) {}

    virtual void OnEnable() override {}
    virtual void OnDisable() override {}
};