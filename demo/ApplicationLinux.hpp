//
//  SoftwareRenderer
//

#pragma once

#include <X11/Xlib.h>
#include "Application.hpp"
#include "Window.hpp"

namespace demo
{
    class WindowLinux: public Window
    {
    public:
        WindowLinux(Application& initApplication);
        virtual ~WindowLinux();

        void draw();
        void didResize(int newWidth, int newHeight);

        inline Display* getDisplay() const { return display; }
        inline ::Window getWindow() const { return window; }
        inline Atom getProtocolsAtom() const { return protocolsAtom; }
        inline Atom getDeleteAtom() const { return deleteAtom; }

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
