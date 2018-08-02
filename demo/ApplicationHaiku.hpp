//
//  SoftwareRenderer
//

#pragma once

#include "Application.hpp"
#include "Window.hpp"

namespace demo
{
    class ApplicationHaiku: public Application
    {
    public:
        ApplicationHaiku(Application& initApplication);
        virtual ~ApplicationHaiku();

        void draw();
        void didResize(int newWidth, int newHeight);

        virtual void run() override;

    private:
    };
}
