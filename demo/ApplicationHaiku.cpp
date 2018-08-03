//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>
#include <Window.h>
#include "ApplicationHaiku.hpp"

namespace demo
{
    ApplicationHaiku::ApplicationHaiku():
    	BApplication("application/x-vnd.SoftwareRenderer")
    {
        BRect frame(100, 100, 400, 400);
        window = new BWindow(frame, "SoftwareRenderer", B_TITLED_WINDOW,
                             B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE);
        window->Show();
    }

    ApplicationHaiku::~ApplicationHaiku()
    {
        // TODO: implement
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
    	Run();
    }

	void ApplicationHaiku::MessageReceived(BMessage* msg)
	{
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
