//
//  SoftwareRenderer
//

#ifndef APPLICATIONANDROID_H
#define APPLICATIONANDROID_H

#include "Application.hpp"

namespace demo
{
    class ApplicationAndroid: public Application
    {
    public:
        ApplicationAndroid();
        virtual ~ApplicationAndroid();

        void draw();
        void didResize(float newWidth, float newHeight);

        void run();
    };
}

#endif
