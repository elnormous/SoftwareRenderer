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
    int i;
    int result;
    const char* outputFile = "output.txt";

#ifdef WIN32
    // TODO: implement command line parsing
    int argc = 0;
    const char** argv = NULL;
#else
    for (i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-o") == 0)
        {
            if (++i < argc) outputFile = argv[i];
        }
    }
#endif

    gpApplicationInit(&application, argc, argv);
    result = gpApplicationRun(&application) ? EXIT_SUCCESS : EXIT_FAILURE;
    gpApplicationDestroy(&application);

    return result;
}
