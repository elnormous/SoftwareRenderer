//
//  SoftwareRenderer
//

#include <Strsafe.h>
#include "Application.hpp"
#include "WindowWindows.hpp"

Application::Application(int initArgc, const char** initArgv):
    argc(initArgc),
    argv(initArgv)
{
}

Application::~Application()
{
}

bool Application::init()
{
    return true;
}

bool Application::run()
{
    WindowWindows* windowWindows = new WindowWindows(*this);
    window.reset(windowWindows);
    if (!windowWindows->init(argc, argv))
        return false;

    MSG msg;
    BOOL ret;
    for (;;)
    {
        ret = GetMessage(&msg, windowWindows->getWindow(), 0, 0);

        if (ret > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else if (ret < 0)
        {
            // error
            return false;
        }
        else
        {
            break;
        }
    }

    return true;
}

std::string Application::getResourcePath() const
{
    return "Resources";
}
