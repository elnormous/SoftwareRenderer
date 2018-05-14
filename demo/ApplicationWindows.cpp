//
//  SoftwareRenderer
//

#include <Strsafe.h>
#include "application.h"
#include "window_windows.h"

void gpMain(GPApplication* application);

int gpApplicationInit(GPApplication* application, int argc, const char** argv)
{
    application->argc = argc;
    application->argv = argv;

    return 1;
}

int gpApplicationDestroy(GPApplication* application)
{
    return 1;
}

int gpApplicationRun(GPApplication* application)
{
    if (!gpWindowInit(&application->window, application->argc, application->argv))
        return 0;

    gpMain(application);

    GPWindowWindows* windowWindows = (GPWindowWindows*)application->window.opaque;
    MSG msg;
    BOOL ret;
    for (;;)
    {
        ret = GetMessage(&msg, windowWindows->window, 0, 0);

        if (ret > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else if (ret < 0)
        {
            // error
            gpWindowDestroy(&application->window);
            return 0;
        }
        else
        {
            break;
        }
    }

    gpWindowDestroy(&application->window);

    return 1;
}
