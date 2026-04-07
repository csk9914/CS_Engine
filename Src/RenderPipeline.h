#pragma once
#include <vector>
#include <d3d11.h>

class DX11Renderer;
class Camera;
class MeshRenderer;

struct RenderViewRequest
{
    Camera*                 targetCamera = nullptr;
    ID3D11RenderTargetView* rtv          = nullptr;
    float                   clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    bool                    isEditorMode  = false;
};

class RenderPipeline
{
public:
    RenderPipeline(DX11Renderer* renderer) : m_renderer(renderer) {}

    void PushRequest(const RenderViewRequest& req);
    void Execute(const std::vector<MeshRenderer*>& meshes);

private:
    DX11Renderer*                m_renderer;
    std::vector<RenderViewRequest> m_requests;
};
