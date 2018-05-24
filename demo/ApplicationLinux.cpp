//
//  SoftwareRenderer
//

#include <iostream>
#include <X11/Xutil.h>
#include "ApplicationLinux.hpp"

namespace demo
{
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
        visual = DefaultVisual(display, screenIndex);
        depth = DefaultDepth(display, screenIndex);

        width = static_cast<uint32_t>(XWidthOfScreen(screen) * 0.6F);
        height = static_cast<uint32_t>(XHeightOfScreen(screen) * 0.6F);

        XSetWindowAttributes swa;
        swa.background_pixel = XWhitePixel(display, screenIndex);
        swa.border_pixel = 0;
        swa.event_mask = KeyPressMask | ExposureMask | StructureNotifyMask;

        window = XCreateWindow(display,
            RootWindow(display, screenIndex),
            0, 0, width, height,
            0, depth, InputOutput, visual,
            CWBorderPixel | CWBackPixel | CWEventMask, &swa);

        XSetStandardProperties(display,
            window, "SoftwareRenderer", "SoftwareRenderer", None,
            const_cast<char**>(argv), argc, NULL);

        XMapWindow(display, window);

        protocolsAtom = XInternAtom(display, "WM_PROTOCOLS", False);
        deleteAtom = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &deleteAtom, 1);

        gc = XCreateGC(display, window, 0, 0);
        XSetForeground(display, gc, 0);

        return Window::init(argc, argv);
    }

    void WindowLinux::draw()
    {
        render();

        const uint8_t* data = renderer.getFrameBuffer().getData().data();
        XImage* image = XCreateImage(display, visual, depth, ZPixmap, 0,
                                     const_cast<char*>(reinterpret_cast<const char*>(data)),
                                     width, height, 32, 0);

        XPutImage(display, window, gc, image, 0, 0, 0, 0, width, height);
        XFlush(display);
        XFree(image);
    }

    void WindowLinux::didResize(int width, int height)
    {
        width = static_cast<uint32_t>(width);
        height = static_cast<uint32_t>(height);

        onResize();
    }

    Application::Application(int initArgc, const char** initArgv):
        argc(initArgc),
        argv(initArgv)
    {
    }

    Application::~Application()
    {
    }

    bool Application::run()
    {
        if (!XInitThreads())
        {
            std::cerr << "Failed to initialize thread support" << std::endl;
            return false;
        }

        WindowLinux* windowLinux = new WindowLinux(*this);
        window.reset(windowLinux);
        windowLinux->init(argc, argv);

        int running = 1;
        XEvent event;

        while (running)
        {
            while (XPending(windowLinux->getDisplay()))
            {
                XNextEvent(windowLinux->getDisplay(), &event);

                switch (event.type)
                {
                    case ClientMessage:
                        if (event.xclient.message_type == windowLinux->getProtocolsAtom() &&
                            event.xclient.data.l[0] == windowLinux->getDeleteAtom())
                            running = 0;
                        break;
                    case KeyPress:
                        break;
                    case Expose:
                        windowLinux->draw();
                        break;
                    case ConfigureNotify:
                        windowLinux->didResize(event.xconfigure.width, event.xconfigure.height);
                        break;
                }
            }

            windowLinux->draw();
        }

        return 1;
    }

    std::string Application::getResourcePath() const
    {
        return "Resources";
    }
}

int main(int argc, const char* argv[])
{
    demo::Application application(argc, argv);

    return application.run() ? EXIT_SUCCESS : EXIT_FAILURE;
}
