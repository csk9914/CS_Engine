#pragma once
#include <Windows.h>
#include <memory>
#include <string>

#include "IRenderer.h"
#include "IRender.h"

class Renderer : public IRenderer
{
private:
	Renderer() : m_width(0), m_height(0), m_bufIdx(0)
	{
		m_buffer[0] = nullptr;
		m_buffer[1] = nullptr;
	}

public:
	~Renderer()
	{
		// 핸들 닫아줌
		if (m_buffer[0]) CloseHandle(m_buffer[0]);
		if (m_buffer[1]) CloseHandle(m_buffer[1]);
	}

	static std::unique_ptr<Renderer> CreateRenderer(int width, int height, bool visible)
	{
		std::unique_ptr<Renderer> renderer = std::unique_ptr<Renderer>(new Renderer());
		if (!renderer->Init(width, height, visible))
		{
			return nullptr;
		}

		return renderer;
	}

	bool Init(int width, int height, bool visible);
	void SetCursorVisible(HANDLE buf, bool visible);

	virtual void Flip() override;
	void ClearScreen();

	void RenderText(int x, int y, const std::string& text);

private:
	// 더블 버퍼 핸들
	HANDLE m_buffer[2];		
	
	int m_width;
	int m_height;
	int m_bufIdx;

};
