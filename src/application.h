//
//  SoftwareRenderer
//

#pragma once

#include "window.h"

typedef struct GPApplication
{
    GPWindow window;
    int argc;
    const char** argv;
} GPApplication;

int gpApplicationInit(GPApplication* application, int argc, const char** argv);
int gpApplicationDestroy(GPApplication* application);
int gpApplicationRun(GPApplication* application);
