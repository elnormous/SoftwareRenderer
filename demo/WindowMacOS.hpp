//
//  SoftwareRenderer
//

#pragma once

#import <Cocoa/Cocoa.h>
#include "Window.hpp"

class WindowMacOS: public Window
{
public:
    ~WindowMacOS();
    bool init(int argc, const char** argv);

private:
    NSScreen* screen;
    NSWindow* window;
    NSView* content;
    NSObject<NSWindowDelegate>* windowDelegate;
};
