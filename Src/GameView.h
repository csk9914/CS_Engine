#pragma once
#include "EditorWindow.h"

class GameView : public EditorWindow
{
public:
    GameView();
    virtual ~GameView() override;

    virtual void Init() override;
    virtual void Update(float deltaTime, RenderPipeline* pipeline, DX11Renderer* renderer) override;
    virtual void OnGUI() override;
};