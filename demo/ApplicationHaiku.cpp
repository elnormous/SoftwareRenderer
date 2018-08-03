//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>
#include <string>
#include <Window.h>
#include "ApplicationHaiku.hpp"

namespace demo
{
    class AppWindow: public BWindow
    {
    public:
        AppWindow(const BRect& frame, const std::string& title):
            BWindow(frame, title.c_str(), B_TITLED_WINDOW,
                    B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE)
        {
        }

        virtual void FrameResized(float newWidth, float newHeight) override
        {
        }
    };

    ApplicationHaiku::ApplicationHaiku():
        BApplication("application/x-vnd.SoftwareRenderer")
    {
        BRect frame(100, 100, 400, 400);
        window = new AppWindow(frame, "SoftwareRenderer");
        window->Show();

        width = 400;
        height = 400;
    }

    ApplicationHaiku::~ApplicationHaiku()
    {
    }

    void ApplicationHaiku::draw()
    {
        render();

        // TODO: implement
    }

    void ApplicationHaiku::didResize(int newWidth, int newHeight)
    {
        width = static_cast<uint32_t>(newWidth);
        height = static_cast<uint32_t>(newHeight);

        onResize();
    }

    void ApplicationHaiku::run()
    {
        setup();

        Run();
    }

    void ApplicationHaiku::MessageReceived(BMessage* msg)
    {
        printf("%ld\n", msg->what);

        switch (msg->what)
        {
            //case A:
            //    break;
            default:
                BApplication::MessageReceived(msg);
                break;
        }
    }

    std::string Application::getResourcePath()
    {
        return "Resources";
    }
}

int main()
{
    try
    {
        demo::ApplicationHaiku application;
        application.run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }
    catch (...)
    {
        return EXIT_FAILURE;
    }
}
