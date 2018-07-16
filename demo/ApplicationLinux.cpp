//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>
#include <X11/Xutil.h>
#include "ApplicationLinux.hpp"

namespace demo
{
    WindowLinux::WindowLinux(Application& initApplication):
        Window(initApplication)
    {
        display = XOpenDisplay(NULL);

        if (!display)
            throw std::runtime_error("Failed to open display");

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
            nullptr, 0, NULL);

        XMapWindow(display, window);

        protocolsAtom = XInternAtom(display, "WM_PROTOCOLS", False);
        deleteAtom = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &deleteAtom, 1);

        gc = XCreateGC(display, window, 0, 0);
        XSetForeground(display, gc, 0);

        init();
    }

    WindowLinux::~WindowLinux()
    {
        if (display)
        {
            if (gc) XFreeGC(display, gc);

            if (window)
                XDestroyWindow(display, window);

            XCloseDisplay(display);
        }
    }

    void WindowLinux::draw()
    {
        render();

        const sr::Texture& frameBuffer = renderTarget.getFrameBuffer();

        const uint8_t* data = frameBuffer.getData().data();
        XImage* image = XCreateImage(display, visual, depth, ZPixmap, 0,
                                     const_cast<char*>(reinterpret_cast<const char*>(data)),
                                     frameBuffer.getWidth(), frameBuffer.getHeight(), 32, 0);

        XPutImage(display, window, gc, image, 0, 0, 0, 0,
                  frameBuffer.getWidth(), frameBuffer.getHeight());
        XFlush(display);
        XFree(image);
    }

    void WindowLinux::didResize(int newWidth, int newHeight)
    {
        width = static_cast<uint32_t>(newWidth);
        height = static_cast<uint32_t>(newHeight);

        onResize();
    }

    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        if (!XInitThreads())
            throw std::runtime_error("Failed to initialize thread support");

        WindowLinux* windowLinux = new WindowLinux(*this);
        window.reset(windowLinux);

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
    }

    std::string Application::getResourcePath()
    {
        return "Resources";
    }
}

int main()
{
    try
    {
        demo::Application application;
        application.run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }
    catch (...)
    {
        return EXIT_FAILURE;
    }
}
