//
//  SoftwareRenderer
//

#pragma once

#include <X11/Xlib.h>
#include "window.h"

typedef struct GPWindowLinux
{
    Display* display;
    Window window;
    Atom protocolsAtom;
    Atom deleteAtom;
    GC gc;
    XFontStruct* font;
    char* text;
    size_t textSize;
} GPWindowLinux;
