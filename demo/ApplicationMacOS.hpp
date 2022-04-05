//
//  SoftwareRenderer
//

#ifndef APPLICATIONMACOS_HPP
#define APPLICATIONMACOS_HPP

#import <Cocoa/Cocoa.h>
#include "Application.hpp"

namespace demo
{
    namespace objc
    {
        template <class T = NSObject>
        class Pointer final
        {
        public:
            Pointer() noexcept = default;

            ~Pointer()
            {
                if (p) [p release];
            }

            Pointer(T* a) noexcept: p{a} {}
            Pointer& operator=(T* a) noexcept
            {
                if (p) [p release];
                p = a;
                return *this;
            }

            operator T*() const noexcept { return p; }
            bool operator!() const noexcept { return p == nil; }

        private:
            T* p = nil;
        };
    }

    class ApplicationMacOS: public Application
    {
    public:
        ApplicationMacOS();
        ~ApplicationMacOS();

        void draw();
        void didResize();

        void run();

    private:
        objc::Pointer<NSAutoreleasePool> pool;

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
