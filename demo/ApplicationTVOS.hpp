//
//  SoftwareRenderer
//

#ifndef APPLICATIONIOS_HPP
#define APPLICATIONIOS_HPP

#import <UIKit/UIKit.h>
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
    
    class ApplicationTVOS: public Application
    {
    public:
        ApplicationTVOS();
        ~ApplicationTVOS();

        void createWindow();

        void draw();
        void didResize(CGFloat newWidth, CGFloat newHeight);

        void run(int argc, char* argv[]);

    private:
        NSAutoreleasePool* pool = nil;

        UIScreen* screen = nil;
        UIWindow* window = nil;
        UIView* content = nil;
        UIViewController* viewController = nil;
        NSTimer* timer = nil;

        std::size_t componentsPerPixel;
        std::size_t bitsPerComponent;
        CGColorSpaceRef colorSpace;
        CGDataProviderRef provider;
    };
}

#endif
