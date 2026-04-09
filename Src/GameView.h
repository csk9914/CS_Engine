#pragma once
#include "EditorWindow.h"

class GameView : public EditorWindow
{
public:
    GameView(EditorUI* editorUI, DX11Renderer* renderer);
    virtual ~GameView() override;

    virtual void Init() override;
    virtual void Update(float deltaTime, RenderPipeline* pipeline) override;
    virtual void OnGUI() override;
};