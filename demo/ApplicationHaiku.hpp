//
//  SoftwareRenderer
//

#ifndef APPLICATIONHAIKU_H
#define APPLICATIONHAIKU_H

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
        ~ApplicationHaiku();

        void draw();
        void didResize(float newWidth, float newHeight);

        void run();

        virtual void Pulse() override;

    private:
        BWindow* window = nullptr;
        AppView* view = nullptr;
        BBitmap* bitmap = nullptr;
    };
}

#endif
