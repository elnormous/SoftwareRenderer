//
//  SoftwareRenderer
//

#pragma once

#import <Cocoa/Cocoa.h>
#include "Application.hpp"
#include "Window.hpp"

namespace demo
{
    class WindowMacOS: public Window
    {
    public:
        WindowMacOS(Application& initApplication);
        virtual ~WindowMacOS();
        virtual bool init(int argc, const char** argv) override;

        void draw();
        void didResize();

    private:
        NSScreen* screen = nil;
        NSWindow* window = nil;
        NSView* content = nil;
        NSObject<NSWindowDelegate>* windowDelegate = nil;
        NSTimer* timer = nil;

        size_t componentsPerPixel;
        size_t bitsPerComponent;
        CGColorSpaceRef colorSpace;
        CGDataProviderRef provider;
    };
}