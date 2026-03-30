#pragma once

class IRenderer
{
public:
	virtual ~IRenderer() = default;
	virtual void Flip() = 0;
};