//
//  SoftwareRenderer
//

#ifndef APPLICATIONX11_H
#define APPLICATIONX11_H

#include <X11/Xlib.h>
#include "Application.hpp"

namespace demo
{
    class ApplicationX11: public Application
    {
    public:
        ApplicationX11();
        virtual ~ApplicationX11();

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

#endif
