//
//  SoftwareRenderer
//

#ifndef APPLICATIONIOS_HPP
#define APPLICATIONIOS_HPP

#import <UIKit/UIKit.h>
#include "Application.hpp"

namespace demo
{
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
