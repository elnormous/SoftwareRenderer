//
//  SoftwareRenderer
//

#ifndef APPLICATIONIOS_H
#define APPLICATIONIOS_H

#import <UIKit/UIKit.h>
#include "Application.hpp"

namespace demo
{
    class ApplicationTVOS: public Application
    {
    public:
        ApplicationTVOS();
        virtual ~ApplicationTVOS();

        void createWindow();

        void draw();
        void didResize(CGFloat newWidth, CGFloat newHeight);

        void run();

    private:
        NSAutoreleasePool* pool = nil;

        UIScreen* screen = nil;
        UIWindow* window = nil;
        UIView* content = nil;
        UIViewController* viewController = nil;
        NSTimer* timer = nil;

        size_t componentsPerPixel;
        size_t bitsPerComponent;
        CGColorSpaceRef colorSpace;
        CGDataProviderRef provider;
    };
}

#endif
