//
//  SoftwareRenderer
//

#include <Strsafe.h>
#include "Application.hpp"
#include "WindowWindows.hpp"

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
        WindowWindows* windowWindows = new WindowWindows(*this);
        window.reset(windowWindows);
        if (!windowWindows->init(argc, argv))
            return false;

        MSG msg;
        for (;;)
        {
            for (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT) return true;
            }

            InvalidateRect(windowWindows->getWindow(), nullptr, FALSE);
        }

        return true;
    }

    std::string Application::getResourcePath() const
    {
        return "Resources";
    }
}
