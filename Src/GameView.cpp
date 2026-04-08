#include "GameView.h"
#include "GameEngine.h"
#include "Camera.h"
#include "RenderPipeline.h"
#include "DX11Renderer.h"
#include "imgui.h"
#include "RenderManager.h"
#include "GameObject.h"

GameView::GameView() : EditorWindow("Game View") {}

GameView::~GameView()
{
}

void GameView::Init()
{
}

void GameView::Update(float deltaTime, RenderPipeline* pipeline, DX11Renderer* renderer)
{
    // 1. 씬에서 MainCamera를 찾음
    Camera* gameCam = GameEngine::Instance()->GetGameCamera();
    if (!gameCam || !pipeline || !renderer) return;

    // [추가] GameView 창 크기에 맞는 Aspect Ratio 계산
    // (만약 창 크기를 가져오기 힘들다면 일단 16:9 고정값이라도 넣어보세요)
    float aspect = (float)renderer->GetWidth() / (float)renderer->GetHeight();

    // [핵심] 게임 카메라의 View/Proj 행렬을 최신화!
    // 이 호출이 없으면 MeshRenderer가 텅 빈 행렬을 가져갑니다.
    gameCam->UpdateMatrix(aspect);

    // 2. 렌더 요청 작성
    RenderViewRequest req;
    req.targetCamera = gameCam;
    req.rtv = renderer->GetGameRTV(); // 게임용 RTV
    req.clearColor[0] = 0.2f; // 게임 화면 배경색
    req.clearColor[1] = 0.2f;
    req.clearColor[2] = 0.2f;
    req.clearColor[3] = 1.0f;
    req.isEditorMode = false; // 에디터 전용 요소(Grid, Gizmo 등) 제외 옵션

    pipeline->PushRequest(req);
}

void GameView::OnGUI()
{
    ImGui::Begin("Game View");

    auto renderer = GameEngine::Instance()->GetRenderManager()->GetRenderer();
    ID3D11ShaderResourceView* gameSRV = renderer->GetGameSRV();

    Camera* gameCam = GameEngine::Instance()->GetGameCamera();
    if (gameCam->GetGameObject()->GetActive())
    {
        if (gameSRV)
        {
            ImVec2 size = ImGui::GetContentRegionAvail();
            // 텍스처를 윈도우 크기에 맞게 출력
            ImGui::Image((void*)gameSRV, size);
        }
    }
    else
    {
        // 화면 중앙에 안내 문구 출력
        ImVec2 size = ImGui::GetContentRegionAvail();
        ImGui::InvisibleButton("EmptyDisplay", size); // 영역 확보

        // 텍스트 정중앙 배치
        float textWidth = ImGui::CalcTextSize("No Active Main Camera").x;
        ImGui::SetCursorPos(ImVec2((size.x - textWidth) * 0.5f, size.y * 0.5f));
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No Active Main Camera");
    }
    ImGui::End();
}
