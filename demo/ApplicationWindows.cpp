//
//  SoftwareRenderer
//

#include <Strsafe.h>
#include "ApplicationWindows.hpp"

static LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    demo::WindowWindows* windowWindows = (demo::WindowWindows*)GetWindowLongPtr(window, GWLP_USERDATA);
    if (!windowWindows) return DefWindowProcW(window, msg, wParam, lParam);

    switch (msg)
    {
        case WM_PAINT:
        {
            windowWindows->draw();
            break;
        }

        case WM_SIZE:
        {
            windowWindows->didResize();
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
            std::cerr << "Failed to register window class" << std::endl;
            return false;
        }

        DWORD windowStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_BORDER | WS_DLGFRAME | WS_THICKFRAME | WS_GROUP | WS_TABSTOP | WS_SIZEBOX | WS_MAXIMIZEBOX;
        DWORD windowExStyle = WS_EX_APPWINDOW;

        window = CreateWindowExW(windowExStyle, WINDOW_CLASS_NAME, L"SoftwareRenderer", windowStyle,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, instance, NULL);

        if (!window)
        {
            std::cerr << "Failed to create window" << std::endl;
            return false;
        }

        RECT clientRect;
        GetClientRect(window, &clientRect);

        width = clientRect.right - clientRect.left;
        height = clientRect.bottom - clientRect.top;

        ShowWindow(window, SW_SHOW);
        SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)this);

        return Window::init(argc, argv);
    }

    void WindowWindows::draw()
    {
        render();

        const sr::Buffer& buffer = renderer.getFrameBuffer();

        BITMAPINFO info;
        ZeroMemory(&info, sizeof(BITMAPINFO));
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = buffer.getWidth();
        info.bmiHeader.biHeight = -(int)buffer.getHeight();
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        info.bmiHeader.biSizeImage = buffer.getWidth() * buffer.getHeight() * 4;

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(window, &ps);

        SetDIBitsToDevice(hdc, 0, 0, buffer.getWidth(), buffer.getHeight(), 0, 0, 0, buffer.getHeight(), buffer.getData().data(), &info, DIB_RGB_COLORS);

        EndPaint(window, &ps);
    }

    void WindowWindows::didResize()
    {
        RECT clientRect;
        GetClientRect(window, &clientRect);

        width = clientRect.right - clientRect.left;
        height = clientRect.bottom - clientRect.top;

        onResize();
    }

    Application::Application(int initArgc, const char** initArgv):
        argc(initArgc),
        argv(initArgv)
    {
    }

    Application::~Application()
    {
    }

    bool Application::run()
    {
        WindowWindows* windowWindows = new WindowWindows(*this);
        window.reset(windowWindows);
        if (!windowWindows->init(argc, argv))
            return false;

        MSG msg;
        for (;;)
        {
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT) return true;
            }

            InvalidateRect(windowWindows->getWindow(), nullptr, FALSE);
        }

        return true;
    }

    std::string Application::getResourcePath() const
    {
        return "Resources";
    }
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    int argc = 0;
    const char** argv = nullptr;

    demo::Application application(argc, argv);

    return application.run() ? EXIT_SUCCESS : EXIT_FAILURE;
}
