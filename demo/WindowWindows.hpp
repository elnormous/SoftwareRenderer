//
//  SoftwareRenderer
//

#pragma once

#include <Windows.h>
#include "Window.hpp"

class WindowWindows: public Window
{
public:
    WindowWindows(Application& initApplication);
    virtual ~WindowWindows();
    virtual bool init(int argc, const char** argv) override;

    void didResize();

    HWND getWindow() const { return window; }
private:
    ATOM windowClass;
    HWND window;
};
