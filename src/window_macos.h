//
//  SoftwareRenderer
//

#pragma once

#import <Cocoa/Cocoa.h>
#include "window.h"

typedef struct GPWindowMacOS
{
    NSScreen* screen;
    NSWindow* window;
    NSView* content;
    NSObject<NSWindowDelegate>* windowDelegate;
} GPWindowMacOS;
