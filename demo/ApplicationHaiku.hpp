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
        virtual ~ApplicationHaiku();

        void draw();
        void didResize(float newWidth, float newHeight);

        virtual void run() override;

        virtual void Pulse() override;

    private:
        BWindow* window = nullptr;
        AppView* view = nullptr;
        BBitmap* bitmap = nullptr;
    };
}

#endif
