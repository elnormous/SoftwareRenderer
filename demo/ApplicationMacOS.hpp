//
//  SoftwareRenderer
//

#ifndef APPLICATIONMACOS_HPP
#define APPLICATIONMACOS_HPP

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
