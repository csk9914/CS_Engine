#pragma once
#include "Component.h"

class MonoBehaviour : public Component
{
public:
    MonoBehaviour() {}
    virtual ~MonoBehaviour() {}

    virtual void Awake() {}
    virtual void Start() {}
    virtual void Update(float deltaTime) {}

    virtual void OnEnable() override {}
    virtual void OnDisable() override {}
};