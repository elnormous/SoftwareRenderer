//
//  SoftwareRenderer
//

#include <functional>
#include <iostream>
#include <thread>
#include "sr.hpp"
#include "Application.hpp"

class Demo
{
public:
    Demo():
        done(false)
    {
        renderer.init(200, 200);
    }

    Demo(const Demo&) = delete;
    Demo& operator=(const Demo&) = delete;
    Demo(Demo&&) = delete;
    Demo& operator=(Demo&&) = delete;

    ~Demo()
    {
        done = true;
        thread.join();
    }

    void run()
    {
        thread = std::thread([this]() {
            while (!done)
            {
                renderer.clear();
                renderer.draw();
            }
        });
    }

    std::thread thread;
    std::condition_variable startCondition;
    std::mutex startMutex;
    std::atomic<bool> done;
    sr::Renderer renderer;
};

Demo demo;

void srMain(Application& application)
{
    demo.run();
}

#ifdef WIN32
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    int argc;
    LPWSTR argv = CommandLineToArgvW(cmdLine, &argc);
#else
int main(int argc, const char* argv[])
{
#endif
    Application application(argc, argv);
    int result;

    application.init();
    result = application.run() ? EXIT_SUCCESS : EXIT_FAILURE;

    return result;
}
