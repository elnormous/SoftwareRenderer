//
//  SoftwareRenderer
//

#pragma once

#include "Application.hpp"
#include "Window.hpp"

namespace demo
{
    class WindowHaiku: public Window
    {
    public:
        WindowHaiku(Application& initApplication);
        virtual ~WindowHaiku();

        void draw();
        void didResize(int newWidth, int newHeight);

    private:
    };
}
