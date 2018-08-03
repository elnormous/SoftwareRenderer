//
//  SoftwareRenderer
//

#pragma once

#include <Application.h>
#include "Application.hpp"

namespace demo
{
    class AppWindow;
    class AppView;

    class ApplicationHaiku: public Application, public BApplication
    {
    public:
        ApplicationHaiku();
        virtual ~ApplicationHaiku();

        void draw();
        void didResize(float newWidth, float newHeight);

        virtual void run() override;

        virtual void MessageReceived(BMessage* msg) override;

    private:
        BWindow* window = nullptr;
        AppView* view = nullptr;
        BBitmap* bitmap = nullptr;
    };
}
