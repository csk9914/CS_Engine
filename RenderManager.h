#pragma once
#include <memory>
#include <vector>

class SpriteRenderer;
class Renderer;
class RenderManager
{
public:
	RenderManager(int width, int height, bool visible);

public:
	void RegisterSprite(SpriteRenderer* render);
	void UnregisterSprite(SpriteRenderer* render);
	void RenderAll();

public:
	Renderer* GetRenderer() const { return m_renderer.get(); }

private:
	std::unique_ptr<Renderer> m_renderer;
	std::vector<SpriteRenderer*> m_sprites;

};