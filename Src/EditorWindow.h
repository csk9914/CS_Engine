#pragma once
#include <string>

class RenderPipeline;
class DX11Renderer;
class EditorUI;

class EditorWindow
{
public:
	EditorWindow(const std::string& name, EditorUI* editorUI, DX11Renderer* renderer) : m_name(name), m_editorUI(editorUI), m_renderer(renderer) {}
	virtual ~EditorWindow() {}
	virtual void Init() = 0;
	virtual void Update(float deltaTime, RenderPipeline* pipeline) = 0;
	virtual void OnGUI() = 0;

protected:
	std::string m_name;
	EditorUI* m_editorUI;

	DX11Renderer* m_renderer;

};