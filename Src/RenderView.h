#pragma once
#include "Camera.h"
#include <d3d11.h>

struct RenderViewContext
{
    Camera* targetCamera;          // 렌더링 시 사용할 카메라 (View/Proj 행렬 추출용)
    ID3D11RenderTargetView* rtv;   // 결과물을 담을 텍스처 (SceneRTV 또는 GameRTV)
    bool isEditorMode;             // 기즈모, 그리드 등을 추가로 그릴지 여부
};