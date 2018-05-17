//
//  SoftwareRenderer
//

#include <stdio.h>
#include <stdlib.h>
#include "WindowWindows.hpp"
#include "Application.hpp"

static LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    WindowWindows* windowWindows = (WindowWindows*)GetWindowLongPtr(window, GWLP_USERDATA);
    if (!windowWindows) return DefWindowProcW(window, msg, wParam, lParam);

    switch (msg)
    {
        case WM_SIZE:
        {
            // TODO: resize back buffer
            break;
        }
    }

    return DefWindowProcW(window, msg, wParam, lParam); 
}

static const LPCWSTR WINDOW_CLASS_NAME = L"SoftwareRenderer";

WindowWindows::WindowWindows(Application& initApplication):
    Window(initApplication)
{
}

WindowWindows::~WindowWindows()
{
    if (window) DestroyWindow(window);
    if (windowClass) UnregisterClassW(WINDOW_CLASS_NAME, GetModuleHandleW(NULL));
}

bool WindowWindows::init(int argc, const char** argv)
{
    HINSTANCE instance = GetModuleHandleW(NULL);

    WNDCLASSEXW wc;
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = windowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = instance;
    // Application icon should be the first resource
    //wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(101));
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hIconSm = NULL;

    windowClass = RegisterClassExW(&wc);
    if (!windowClass)
    {
        fprintf(stderr, "Failed to register window class\n");
        return false;
    }

    DWORD windowStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_BORDER | WS_DLGFRAME | WS_THICKFRAME | WS_GROUP | WS_TABSTOP | WS_SIZEBOX | WS_MAXIMIZEBOX;

    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;

    DWORD windowExStyle = WS_EX_APPWINDOW;

    int width = CW_USEDEFAULT;
    int height = CW_USEDEFAULT;

    window = CreateWindowExW(windowExStyle, WINDOW_CLASS_NAME, L"SoftwareRenderer", windowStyle,
        x, y, width, height, NULL, NULL, instance, NULL);

    if (!window)
    {
        fprintf(stderr, "Failed to create window\n");
        return false;
    }

    RECT clientRect;
    GetClientRect(window, &clientRect);

    ShowWindow(window, SW_SHOW);
    SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)this);

    return true;
}
