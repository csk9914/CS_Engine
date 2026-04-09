#pragma once
#include "EditorWindow.h"
#include <memory>

class GameObject;
class Camera;
class EditorUI;

class SceneView : public EditorWindow
{
public:
    SceneView(EditorUI* editorUI, DX11Renderer* renderer);
    virtual ~SceneView() override;

    virtual void Init() override;
    virtual void Update(float deltaTime, RenderPipeline* pipeline) override;
    virtual void OnGUI() override;

    Camera* GetEditorCamera() const { return m_editorCamera; }

private:
    // Camera 컴포넌트는 반드시 GameObject가 있어야 동작함
    std::unique_ptr<GameObject> m_cameraObject;
    Camera* m_editorCamera = nullptr; // 캐시 포인터 (소유권은 m_cameraObject)

};
