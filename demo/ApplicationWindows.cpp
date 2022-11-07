//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>
#include <Strsafe.h>
#include "ApplicationWindows.hpp"

namespace
{
    LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        auto applicationWindows = reinterpret_cast<demo::ApplicationWindows*>(GetWindowLongPtr(window, GWLP_USERDATA));
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
}

namespace demo
{
    std::string getResourcePath()
    {
        return "Resources";
    }

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
        wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(COLOR_WINDOW));
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = L"SoftwareRenderer";
        wc.hIconSm = nullptr;

        windowClass = RegisterClassExW(&wc);
        if (!windowClass)
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to register window class"};

        const DWORD windowStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_BORDER | WS_DLGFRAME | WS_THICKFRAME | WS_GROUP | WS_TABSTOP | WS_SIZEBOX | WS_MAXIMIZEBOX;
        const DWORD windowExStyle = WS_EX_APPWINDOW;

        window = CreateWindowExW(windowExStyle, MAKEINTATOM(windowClass), L"SoftwareRenderer", windowStyle,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, instance, nullptr);

        if (!window)
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to create window"};

        RECT clientRect;
        GetClientRect(window, &clientRect);

        const auto w = static_cast<std::size_t>(clientRect.right - clientRect.left);
        const auto h = static_cast<std::size_t>(clientRect.bottom - clientRect.top);

        ShowWindow(window, SW_SHOW);
        SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)this);

        setup(w, h);
    }

    ApplicationWindows::~ApplicationWindows()
    {
        if (window) DestroyWindow(window);
        if (windowClass) UnregisterClassW(MAKEINTATOM(windowClass), GetModuleHandleW(nullptr));
    }

    void ApplicationWindows::draw()
    {
        render();

        const auto& frameBuffer = getFrameBuffer();

        BITMAPINFO info = {};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = static_cast<LONG>(frameBuffer.getWidth());
        info.bmiHeader.biHeight = -static_cast<LONG>(frameBuffer.getHeight());
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(window, &ps);

        SetDIBitsToDevice(hdc, 0, 0,
                          static_cast<DWORD>(frameBuffer.getWidth()),
                          static_cast<DWORD>(frameBuffer.getHeight()),
                          0, 0, 0,
                          static_cast<UINT>(frameBuffer.getHeight()),
                          frameBuffer.getData().data(),
                          &info, DIB_RGB_COLORS);

        EndPaint(window, &ps);
    }

    void ApplicationWindows::didResize()
    {
        RECT clientRect;
        GetClientRect(window, &clientRect);

        onResize(static_cast<std::size_t>(clientRect.right - clientRect.left),
                 static_cast<std::size_t>(clientRect.bottom - clientRect.top));
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
