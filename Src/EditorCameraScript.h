#pragma once
#include "MonoBehaviour.h"


class Camera;
class Transform;

class EditorCameraScript : public MonoBehaviour
{
public:
    GameObject* m_obj;
    Transform* m_transform;
    Camera* m_cam;

public:
    virtual void Start() override;

    virtual void Update(float deltaTime) override;

};