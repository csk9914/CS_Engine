#pragma once
#include "MonoBehaviour.h"


class Camera;
class Transform;

class EditorCameraScript : public MonoBehaviour
{
public:
    GameObject* m_obj = nullptr;
    Transform* m_transform = nullptr;
    Camera* m_cam = nullptr;

public:
    virtual void Start() override;

    virtual void Update(float deltaTime) override;

};