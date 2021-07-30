//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>
#include <string>
#include <Bitmap.h>
#include <Window.h>
#include "ApplicationHaiku.hpp"

namespace demo
{
    class AppView: public BView
    {
    public:
        AppView(ApplicationHaiku& initApplication, const BRect& frame, const std::string& title):
            BView(frame, title.c_str(), B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS),
            application(initApplication)
        {
        }

        virtual void FrameResized(float newWidth, float newHeight) override
        {
            application.didResize(newWidth, newHeight);
        }

        virtual void Draw(BRect) override
        {
            application.draw();
        }

    private:
        ApplicationHaiku& application;
    };

    ApplicationHaiku::ApplicationHaiku():
        BApplication("application/x-vnd.SoftwareRenderer")
    {
        const BRect frame{100, 100, 100 + 640, 100 + 480};
        window = new BWindow(frame, "SoftwareRenderer", B_TITLED_WINDOW,
                             B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE);

        const BRect bounds = window->Bounds();

        view = new AppView(*this, bounds, "render");
        window->AddChild(view);

        bitmap = new BBitmap(bounds, 0, B_RGB32);

        const auto w = static_cast<std::size_t>(bounds.Width());
        const auto h = static_cast<std::size_t>(bounds.Height());

        setup(w, h);
        window->Show();
        SetPulseRate(100000);
    }

    ApplicationHaiku::~ApplicationHaiku()
    {
        if (bitmap) delete bitmap;
    }

    void ApplicationHaiku::draw()
    {
        render();

        const auto& frameBuffer = getRenderTarget().getFrameBuffer();
        bitmap->ImportBits(frameBuffer.getData().data(), frameBuffer.getWidth() * frameBuffer.getHeight() * 4,
                           frameBuffer.getWidth() * 4, 0, B_RGB32);

        view->DrawBitmap(bitmap, bitmap->Bounds(), view->Bounds(), 0);
    }

    void ApplicationHaiku::didResize(float newWidth, float newHeight)
    {
        if (bitmap) delete bitmap;

        const auto w = static_cast<std::size_t>(newWidth);
        const auto h = static_cast<std::size_t>(newHeight);

        bitmap = new BBitmap(BRect{0, 0, w, h}, 0, B_RGB32);

        onResize(w, h);
    }

    void ApplicationHaiku::run()
    {
        Run();
    }

    void ApplicationHaiku::Pulse()
    {
    	if (window->Lock())
    	{
            view->Invalidate();
            window->Unlock();
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
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        return EXIT_FAILURE;
    }
}
