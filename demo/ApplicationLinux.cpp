//
//  SoftwareRenderer
//

#include <iostream>
#include "Application.hpp"
#include "WindowLinux.hpp"

namespace demo
{
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
