//
//  SoftwareRenderer
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "application.h"
#include "thread.h"

void threadFunc(void* argument)
{
    GPApplication* application = (GPApplication*)argument;
}

void gpMain(GPApplication* application)
{
    GPThread thread;
    GPCondition startCondition;
    GPMutex startMutex;

    gpThreadInit(&thread, threadFunc, application, "Capture");
}

#ifdef WIN32
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
#else
int main(int argc, const char* argv[])
#endif
{
    GPApplication application;
    int result;

    gpApplicationInit(&application, argc, argv);
    result = gpApplicationRun(&application) ? EXIT_SUCCESS : EXIT_FAILURE;
    gpApplicationDestroy(&application);

    return result;
}
