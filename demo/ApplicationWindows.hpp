//
//  SoftwareRenderer
//

#ifndef APPLICATIONWINDOWS_HPP
#define APPLICATIONWINDOWS_HPP

#include <Windows.h>
#include "Application.hpp"

namespace demo
{
    class ApplicationWindows: public Application
    {
    public:
        ApplicationWindows();
        ~ApplicationWindows();

        void draw();
        void didResize();

        void run();

    private:
        ATOM windowClass = 0;
        HWND window = 0;
    };
}

#endif
