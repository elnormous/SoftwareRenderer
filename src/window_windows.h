//
//  SoftwareRenderer
//

#pragma once

#include <Windows.h>
#include "window.h"

typedef struct GPWindowWindows
{
    ATOM windowClass;
    HWND window;
    HWND textBox;
    LPWSTR* text;
    size_t textSize;
} GPWindowWindows;
