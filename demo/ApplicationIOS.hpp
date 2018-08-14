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

        void draw();
        void didResize();

        void run();
    };
}

#endif
