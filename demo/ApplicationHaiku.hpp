//
//  SoftwareRenderer
//

#pragma once

#include "Application.hpp"

namespace demo
{
    class ApplicationHaiku: public Application
    {
    public:
        ApplicationHaiku();
        virtual ~ApplicationHaiku();

        void draw();
        void didResize(int newWidth, int newHeight);

        virtual void run() override;

    private:
    };
}
