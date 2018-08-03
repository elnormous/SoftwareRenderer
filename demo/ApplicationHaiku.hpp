//
//  SoftwareRenderer
//

#pragma once

#include <Application.h>
#include "Application.hpp"

namespace demo
{
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
        int32 count = 0;
    	BWindow* window = nullptr;
    };
}
