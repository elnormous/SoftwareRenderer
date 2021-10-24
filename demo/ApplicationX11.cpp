//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>
#include <X11/Xutil.h>
#include "ApplicationX11.hpp"

namespace demo
{
    std::string getResourcePath()
    {
        return "Resources";
    }

    ApplicationX11::ApplicationX11()
    {
        if (!XInitThreads())
            throw std::runtime_error("Failed to initialize thread support");

        display = XOpenDisplay(nullptr);

        if (!display)
            throw std::runtime_error("Failed to open display");

        Screen* screen = XDefaultScreenOfDisplay(display);
        const int screenIndex = XScreenNumberOfScreen(screen);
        visual = DefaultVisual(display, screenIndex);
        depth = DefaultDepth(display, screenIndex);

        const auto w = static_cast<std::size_t>(XWidthOfScreen(screen) * 0.6F);
        const auto h = static_cast<std::size_t>(XHeightOfScreen(screen) * 0.6F);

        XSetWindowAttributes swa;
        swa.background_pixel = XWhitePixel(display, screenIndex);
        swa.border_pixel = 0;
        swa.event_mask = KeyPressMask | ExposureMask | StructureNotifyMask;

        window = XCreateWindow(display,
                               RootWindow(display, screenIndex),
                               0, 0, w, h,
                               0, depth, InputOutput, visual,
                               CWBorderPixel | CWBackPixel | CWEventMask, &swa);

        XSetStandardProperties(display,
                               window, "SoftwareRenderer", "SoftwareRenderer", None,
                               nullptr, 0, nullptr);

        XMapWindow(display, window);

        protocolsAtom = XInternAtom(display, "WM_PROTOCOLS", False);
        deleteAtom = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &deleteAtom, 1);

        gc = XCreateGC(display, window, 0, 0);
        XSetForeground(display, gc, 0);

        setup(w, h);
    }

    ApplicationX11::~ApplicationX11()
    {
        if (display)
        {
            if (gc) XFreeGC(display, gc);
            if (window) XDestroyWindow(display, window);

            XCloseDisplay(display);
        }
    }

    void ApplicationX11::draw()
    {
        render();

        const auto& frameBuffer = getFrameBuffer();

        const auto data = frameBuffer.getData().data();
        XImage* image = XCreateImage(display, visual, depth, ZPixmap, 0,
                                     const_cast<char*>(reinterpret_cast<const char*>(data)),
                                     frameBuffer.getWidth(), frameBuffer.getHeight(), 32, 0);

        XPutImage(display, window, gc, image, 0, 0, 0, 0,
                  frameBuffer.getWidth(), frameBuffer.getHeight());
        XFlush(display);
        XFree(image);
    }

    void ApplicationX11::didResize(int newWidth, int newHeight)
    {
        onResize(static_cast<std::size_t>(newWidth),
                 static_cast<std::size_t>(newHeight));
    }

    void ApplicationX11::run()
    {
        bool running = true;

        while (running)
        {
            while (XPending(display))
            {
                XEvent event;
                XNextEvent(display, &event);

                switch (event.type)
                {
                    case ClientMessage:
                        if (event.xclient.message_type == protocolsAtom &&
                            static_cast<Atom>(event.xclient.data.l[0]) == deleteAtom)
                            running = false;
                        break;
                    case KeyPress:
                        break;
                    case Expose:
                        draw();
                        break;
                    case ConfigureNotify:
                        didResize(event.xconfigure.width, event.xconfigure.height);
                        break;
                }
            }

            draw();
        }
    }
}

int main()
{
    try
    {
        demo::ApplicationX11 application;
        application.run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        return EXIT_FAILURE;
    }
}
