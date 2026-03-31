#include "Renderer.h"

#include <string>

bool Renderer::Init(int width, int height, bool visible)
{
	m_width = width;
	m_height = height;

	for (int i = 0; i < 2; i++)
	{
		// 버퍼 생성
		m_buffer[i] = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, CONSOLE_TEXTMODE_BUFFER, NULL
		);

		// 창 크기를 최소화 (버퍼 크기를 줄일 때 오류 방지)
		SMALL_RECT minRect = { 0,0, 1,1 };
		SetConsoleWindowInfo(m_buffer[i], true, &minRect);

		// 버퍼 크기 설정
		COORD size = { (SHORT)m_width, (SHORT)m_height };
		SetConsoleScreenBufferSize(m_buffer[i], size);

		// 창 크기를 버퍼 크기와 동일하게 설정
		SMALL_RECT rect = { 0, 0, (SHORT)(m_width - 1), (SHORT)(m_height - 1)};
		SetConsoleWindowInfo(m_buffer[i], true, &rect);

		// 커서 숨기기
		SetCursorVisible(m_buffer[i], visible);
	}

    return true;
}

void Renderer::SetCursorVisible(HANDLE buf, bool visible)
{
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(buf, &cursorInfo);
	cursorInfo.bVisible = visible;
	SetConsoleCursorInfo(buf, &cursorInfo);
}

void Renderer::Flip()
{
	// 다 그려진 백버퍼를 화면으로 전환
	SetConsoleActiveScreenBuffer(m_buffer[m_bufIdx]);

	// 인덱스 전환 (이제 m_bufIdx는 다음 프레임을 그릴 새로운 백버퍼를 가리킴)
	m_bufIdx = (m_bufIdx + 1) % 2;

	// 새로 배정받은 백버퍼를 깨끗이 지움
	ClearScreen();
}

void Renderer::ClearScreen()
{
	COORD startPos = { 0,0 };
	DWORD written;
	DWORD screenSize = m_width * m_height;

	// 현재 백버퍼를 공백으로 꽉 채워서 이전 프레임 잔상 삭제
	FillConsoleOutputCharacterA(m_buffer[m_bufIdx], ' ', screenSize, startPos, & written);
	// 커서도 다시 처음으로
	SetConsoleCursorPosition(m_buffer[m_bufIdx], startPos);
}

void Renderer::RenderText(int x, int y, const std::string& text)
{
	// 현재 그리고 있는 백버퍼 핸들을 가져옴
	HANDLE backBuffer = m_buffer[m_bufIdx];

	// 좌표 설정
	COORD pos;
	pos.X = x;
	pos.Y = y;

	SetConsoleCursorPosition(backBuffer, pos);

	// 백버퍼에 문자열 쓰기
	DWORD written;
	WriteConsoleA(backBuffer, text.c_str(), text.length(), &written, NULL);
}
