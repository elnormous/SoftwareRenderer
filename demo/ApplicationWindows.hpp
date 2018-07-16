//
//  SoftwareRenderer
//

#pragma once

#include <Windows.h>
#include "Application.hpp"
#include "Window.hpp"

namespace demo
{
    class WindowWindows: public Window
    {
    public:
        WindowWindows(Application& initApplication);
        virtual ~WindowWindows();

        void draw();
        void didResize();

        HWND getWindow() const { return window; }
    private:
        ATOM windowClass = 0;
        HWND window = 0;
    };
}
