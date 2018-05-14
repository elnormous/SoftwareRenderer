//
//  SoftwareRenderer
//

#pragma once

#import <Cocoa/Cocoa.h>
#include "Window.hpp"

class WindowMacOS: public Window
{
public:
    WindowMacOS(Application& initApplication);
    virtual ~WindowMacOS();
    virtual bool init(int argc, const char** argv) override;

    void didResize();

private:
    NSScreen* screen = nil;
    NSWindow* window = nil;
    NSView* content = nil;
    NSObject<NSWindowDelegate>* windowDelegate = nil;
    NSTimer* timer = nil;
};
