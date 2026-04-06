#include "CSWindow.h"
#include "InputHandler.h"

#include "imgui/imgui.h"

CSWindow::~CSWindow()
{
    if (m_hWnd)
    {
        DestroyWindow(m_hWnd);
        UnregisterClassW(L"CSEngineWindow", m_hInst);
    }
}

bool CSWindow::Init(const std::wstring& title, int width, int height, HINSTANCE hInstance)
{
    m_hInst = hInstance;
    m_width = width;
    m_height = height;

    // 윈도우 클래스 등록
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = m_hInst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"CSEngineWindow";
    RegisterClassEx(&wc);

    // 실제 클라이언트 영역이 width x height 가 되도록 보정
    RECT wr = { 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    // 윈도우 생성
    m_hWnd = CreateWindowW(
        L"CSEngineWindow", title.c_str(),
        WS_OVERLAPPEDWINDOW,
        100, 100,
        wr.right - wr.left,
        wr.bottom - wr.top,
        nullptr, nullptr, m_hInst, nullptr
    );

    if (!m_hWnd) return false;

    ShowWindow(m_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hWnd);

    return true;
}

bool CSWindow::PumpMessages()
{
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_QUIT) 
            return false;
    }
    return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
LRESULT CSWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_MOUSEWHEEL:
        // InputHandler에 휠 델타 전달
        InputHandler::Instance()->SetWheelDelta(
            (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
