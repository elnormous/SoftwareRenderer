//
//  SoftwareRenderer
//

#ifndef APPLICATIONANDROID_HPP
#define APPLICATIONANDROID_HPP

#include <jni.h>
#include "Application.hpp"

namespace demo
{
    class ApplicationAndroid: public Application
    {
    public:
        ApplicationAndroid(JavaVM* initJavaVM);
        ~ApplicationAndroid();

        void init(jint initWidth, jint initHeight);
        void onDraw(jobject bitmap);
        void onSizeChanged(jint newWidth, jint newHeight);

    private:
        JavaVM* javaVM;
    };
}

#endif
