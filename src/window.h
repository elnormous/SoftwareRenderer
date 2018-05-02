//
//  SoftwareRenderer
//

#pragma once

typedef struct GPWindow
{
    void* opaque;
} GPWindow;

int gpWindowInit(GPWindow* window, int argc, const char** argv);
int gpWindowDestroy(GPWindow* window);
