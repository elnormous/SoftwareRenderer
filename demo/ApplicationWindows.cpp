//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>
#include <Strsafe.h>
#include "ApplicationWindows.hpp"

static LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    demo::ApplicationWindows* applicationWindows = (demo::ApplicationWindows*)GetWindowLongPtr(window, GWLP_USERDATA);
    if (!applicationWindows) return DefWindowProcW(window, msg, wParam, lParam);

    switch (msg)
    {
        case WM_PAINT:
        {
            applicationWindows->draw();
            break;
        }

        case WM_SIZE:
        {
            applicationWindows->didResize();
            break;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
    }

    return DefWindowProcW(window, msg, wParam, lParam); 
}

static const LPCWSTR WINDOW_CLASS_NAME = L"SoftwareRenderer";

namespace demo
{
    ApplicationWindows::ApplicationWindows()
    {
        HINSTANCE instance = GetModuleHandleW(nullptr);

        WNDCLASSEXW wc;
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = windowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = instance;
        // Application icon should be the first resource
        //wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(101));
        wc.hIcon = nullptr;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW);
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = WINDOW_CLASS_NAME;
        wc.hIconSm = nullptr;

        windowClass = RegisterClassExW(&wc);
        if (!windowClass)
            throw std::runtime_error("Failed to register window class");

        DWORD windowStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_BORDER | WS_DLGFRAME | WS_THICKFRAME | WS_GROUP | WS_TABSTOP | WS_SIZEBOX | WS_MAXIMIZEBOX;
        DWORD windowExStyle = WS_EX_APPWINDOW;

        window = CreateWindowExW(windowExStyle, WINDOW_CLASS_NAME, L"SoftwareRenderer", windowStyle,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, instance, nullptr);

        if (!window)
            throw std::runtime_error("Failed to create window");

        RECT clientRect;
        GetClientRect(window, &clientRect);

        width = clientRect.right - clientRect.left;
        height = clientRect.bottom - clientRect.top;

        ShowWindow(window, SW_SHOW);
        SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)this);

        setup();
    }

    ApplicationWindows::~ApplicationWindows()
    {
        if (window) DestroyWindow(window);
        if (windowClass) UnregisterClassW(WINDOW_CLASS_NAME, GetModuleHandleW(nullptr));
    }

    void ApplicationWindows::draw()
    {
        render();

        const auto& frameBuffer = renderTarget.getFrameBuffer();

        BITMAPINFO info;
        ZeroMemory(&info, sizeof(BITMAPINFO));
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = frameBuffer.getWidth();
        info.bmiHeader.biHeight = -static_cast<int>(frameBuffer.getHeight());
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        info.bmiHeader.biSizeImage = frameBuffer.getWidth() * frameBuffer.getHeight() * 4;

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(window, &ps);

        SetDIBitsToDevice(hdc, 0, 0, frameBuffer.getWidth(), frameBuffer.getHeight(), 0, 0, 0, frameBuffer.getHeight(), frameBuffer.getData().data(), &info, DIB_RGB_COLORS);

        EndPaint(window, &ps);
    }

    void ApplicationWindows::didResize()
    {
        RECT clientRect;
        GetClientRect(window, &clientRect);

        width = clientRect.right - clientRect.left;
        height = clientRect.bottom - clientRect.top;

        onResize();
    }

    void ApplicationWindows::run()
    {
        MSG msg;
        for (;;)
        {
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT) return;
            }

            InvalidateRect(window, nullptr, FALSE);
        }
    }

    std::string Application::getResourcePath()
    {
        return "Resources";
    }
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        demo::ApplicationWindows application;
        application.run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        return EXIT_FAILURE;
    }
}
