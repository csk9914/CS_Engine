#pragma once
#include <string>

class RenderPipeline;
class DX11Renderer;

class EditorWindow
{
public:
	EditorWindow(const std::string& name) : m_name(name) {}
	virtual ~EditorWindow() {}
	virtual void Init() = 0;
	virtual void Update(float deltaTime, RenderPipeline* pipeline, DX11Renderer* renderer) = 0;
	virtual void OnGUI() = 0;

protected:
	std::string m_name;
};