//
//  SoftwareRenderer
//

#ifndef APPLICATIONMACOS_H
#define APPLICATIONMACOS_H

#import <Cocoa/Cocoa.h>
#include "Application.hpp"

namespace demo
{
    class ApplicationMacOS: public Application
    {
    public:
        ApplicationMacOS();
        ~ApplicationMacOS();

        void draw();
        void didResize();

        void run();

    private:
        NSAutoreleasePool* pool = nil;

        NSScreen* screen = nil;
        NSWindow* window = nil;
        NSView* content = nil;
        NSObject<NSWindowDelegate>* windowDelegate = nil;
        NSTimer* timer = nil;

        std::size_t componentsPerPixel;
        std::size_t bitsPerComponent;
        CGColorSpaceRef colorSpace;
        CGDataProviderRef provider;
    };
}

#endif
