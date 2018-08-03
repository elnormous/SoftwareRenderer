//
//  SoftwareRenderer
//

#pragma once

#include <Application.h>
#include "Application.hpp"

namespace demo
{
    class AppWindow;

    class ApplicationHaiku: public Application, public BApplication
    {
    public:
        ApplicationHaiku();
        virtual ~ApplicationHaiku();

        void draw();
        void didResize(int newWidth, int newHeight);

        virtual void run() override;

        virtual void MessageReceived(BMessage* msg) override;

    private:
        AppWindow* window = nullptr;
    };
}
