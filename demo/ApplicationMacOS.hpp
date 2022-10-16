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

    namespace cf
    {
        template <class T = CFTypeRef, typename std::enable_if_t<std::is_pointer_v<T>>* = nullptr>
        class Pointer final
        {
        public:
            Pointer() noexcept = default;

            ~Pointer()
            {
                if (p) CFRelease(p);
            }

            Pointer(T a) noexcept: p{a} {}
            Pointer& operator=(T a) noexcept
            {
                if (p) CFRelease(p);
                p = a;
                return *this;
            }

            operator T() const noexcept { return p; }
            bool operator!() const noexcept { return p == nullptr; }

        private:
            T p = nullptr;
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

        objc::Pointer<NSWindow> window;
        objc::Pointer<NSView> content;
        objc::Pointer<NSTimer> timer;

        std::size_t componentsPerPixel;
        std::size_t bitsPerComponent;
        cf::Pointer<CGColorSpaceRef> colorSpace;
        cf::Pointer<CGDataProviderRef> provider;
    };
}

#endif
