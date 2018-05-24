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
        virtual bool init(int argc, const char** argv) override;

        void draw();
        void didResize();

        HWND getWindow() const { return window; }
    private:
        ATOM windowClass;
        HWND window;
    };
}
