//
//  SoftwareRenderer
//

#ifndef APPLICATIONIOS_H
#define APPLICATIONIOS_H

#import <UIKit/UIKit.h>
#include "Application.hpp"

namespace demo
{
    class ApplicationIOS: public Application
    {
    public:
        ApplicationIOS();
        virtual ~ApplicationIOS();

        void createWindow();

        void draw();
        void didResize(CGFloat newWidth, CGFloat newHeight);

        void run();

    private:
        NSAutoreleasePool* pool = nil;

        UIScreen* screen = nil;
        UIWindow* window = nil;
        UIView* view = nil;
        UIViewController* viewController = nil;
    };
}

#endif
