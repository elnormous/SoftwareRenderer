//
//  SoftwareRenderer
//

#ifndef APPLICATIONX11_HPP
#define APPLICATIONX11_HPP

#include <X11/Xlib.h>
#include "Application.hpp"

namespace demo
{
    class ApplicationX11: public Application
    {
    public:
        ApplicationX11();
        ~ApplicationX11();

        void draw();
        void didResize(int newWidth, int newHeight);

        void run();

    private:
        Visual* visual = nullptr;
        int depth;
        Display* display = nullptr;
        ::Window window;
        Atom protocolsAtom;
        Atom deleteAtom;
        GC gc;
    };
}

#endif
