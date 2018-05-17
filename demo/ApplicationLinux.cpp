//
//  SoftwareRenderer
//

#include <iostream>
#include "Application.hpp"
#include "WindowLinux.hpp"

Application::Application(int argc, const char** argv):
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
        }
    }

    return 1;
}
