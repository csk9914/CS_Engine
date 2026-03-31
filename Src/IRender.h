#pragma once

class IRenderer;
class IRender
{
public:
	virtual ~IRender() = default;

	// 렌더러가 'nullptr'일 리가 없음을 보장
	virtual void Render(IRenderer& renderer) const = 0;
};