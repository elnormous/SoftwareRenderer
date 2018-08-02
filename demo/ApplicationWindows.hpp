//
//  SoftwareRenderer
//

#pragma once

#include <Windows.h>
#include "Application.hpp"

namespace demo
{
    class ApplicationWindows: public Application
    {
    public:
        ApplicationWindows();
        virtual ~ApplicationWindows();

        void draw();
        void didResize();

        virtual void run() override;

    private:
        ATOM windowClass = 0;
        HWND window = 0;
    };
}
