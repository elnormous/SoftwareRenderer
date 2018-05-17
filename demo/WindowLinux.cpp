//
//  SoftwareRenderer
//

#include <iostream>
#include <X11/Xutil.h>
#include "window_linux.h"
#include "application.h"

WindowLinux::WindowLinux(Application& initApplication):
    Window(initApplication)
{
}

WindowLinux::~WindowLinux()
{
    if (gc) XFreeGC(display, gc);

    if (display)
    {
        if (window)
            XDestroyWindow(display, window);

        XCloseDisplay(display);
    }
}

bool WindowLinux::init(int argc, const char** argv)
{
    display = XOpenDisplay(NULL);

    if (!display)
    {
        std::cerr << "Failed to open display" << std::endl;
        return false;
    }

    Screen* screen = XDefaultScreenOfDisplay(display);
    int screenIndex = XScreenNumberOfScreen(screen);

    unsigned int width = XWidthOfScreen(screen) * 0.6f;
    unsigned int height = XHeightOfScreen(screen) * 0.6f;

    XSetWindowAttributes swa;
    swa.background_pixel = XWhitePixel(display, screenIndex);
    swa.border_pixel = 0;
    swa.event_mask = KeyPress;

    window = XCreateWindow(display,
        RootWindow(display, screenIndex),
        0, 0, width, height,
        0, DefaultDepth(display, screenIndex), InputOutput,
        DefaultVisual(display, screenIndex),
        CWBorderPixel | CWBackPixel | CWEventMask, &swa);

    XSetStandardProperties(display,
        window, "SoftwareRenderer", "SoftwareRenderer", None,
        argv, argc, NULL);

    XMapWindow(display, window);

    protocolsAtom = XInternAtom(display, "WM_PROTOCOLS", False);
    deleteAtom = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteAtom, 1);

    gc = XCreateGC(display, window, 0, 0);
    XSetForeground(display, gc, 0);

    return true;
}
