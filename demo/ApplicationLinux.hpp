//
//  SoftwareRenderer
//

#pragma once

#include <X11/Xlib.h>
#include "Application.hpp"

namespace demo
{
    class ApplicationLinux: public Window
    {
    public:
        ApplicationLinux();
        virtual ~ApplicationLinux();

        void draw();
        void didResize(int newWidth, int newHeight);

        virtual void run() override;

    private:
        Visual* visual;
        int depth;
        Display* display;
        ::Window window;
        Atom protocolsAtom;
        Atom deleteAtom;
        GC gc;
    };
}
