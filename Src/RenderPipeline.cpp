#include "RenderPipeline.h"
#include "DX11Renderer.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "GameEngine.h"
#include "RenderManager.h"

void RenderPipeline::PushRequest(const RenderViewRequest& req)
{
	m_requests.push_back(req);
}

void RenderPipeline::Execute(const std::vector<MeshRenderer*>& meshes)
{
	if (m_requests.empty()) return;

	auto* ctx = m_renderer->GetContext();
	auto* dsv = m_renderer->GetDSV();

	for (const auto& req : m_requests)
	{
		if (!req.targetCamera || !req.rtv) continue;

		// 1. 렌더 타겟 설정 & 클리어
		float color[4] = { req.clearColor[0], req.clearColor[1],req.clearColor[2], req.clearColor[3] };

		ctx->ClearRenderTargetView(req.rtv, color);
		ctx->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		ctx->OMSetRenderTargets(1, &req.rtv, dsv);

		// Viewport도 RTT 크기에 맞게 설정
		// (현재는 전체 화면 크기와 같으므로 기존 viewport 사용)
		D3D11_VIEWPORT vp = { 0.f, 0.f,(float)m_renderer->GetWidth(),(float)m_renderer->GetHeight(),0.f, 1.f };
		ctx->RSSetViewports(1, &vp);

		// 2. 카메라를 GameEngine의 MainCamera로 임시 교체하여 렌더
		// (MeshRenderer가 GetMainCamera()를 통해 행렬을 가져오기 때문)
		Camera* prevCam = GameEngine::Instance()->GetRenderCamera();
		GameEngine::Instance()->SetRenderCamera(req.targetCamera);

		// 3. Draw Call
		for (auto* mesh : meshes)
			if (mesh) mesh->Render();

		// 4. 카메라 복구
		if (prevCam)
			GameEngine::Instance()->SetRenderCamera(prevCam);
	}

	m_requests.clear();
}
