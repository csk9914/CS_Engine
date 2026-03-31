#pragma once
#include "Component.h"
#include <string>

class Texture;
class SpriteRenderer : public Component
{
public:
	SpriteRenderer() :m_texture(nullptr), m_color{ 255,255,255,255 }, m_orerInLayer(0) {}
	virtual ~SpriteRenderer();

public:
	void Render();

	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void OnDestroy() override;

public:
	// 설정 함수
	void SetTexture(Texture* texture) { m_texture = texture; }
	void SetOrderInLayer(int order) { m_orerInLayer = order; }

	void SetSprite(const std::string& sprite) { m_sprite = sprite; }

private:
	Texture* m_texture;	// 그릴 이미지
	struct Color { unsigned char r, g, b, a; } m_color;

	int m_orerInLayer;	//z-order, 렌더링 순서
	std::string m_sprite;
};