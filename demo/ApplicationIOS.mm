//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>
#include "ApplicationIOS.hpp"

demo:: ApplicationIOS* sharedApplication;

@interface AppDelegate: UIResponder<UIApplicationDelegate>

@end

@implementation AppDelegate

-(BOOL)application:(__unused UIApplication*)application willFinishLaunchingWithOptions:(__unused NSDictionary*)launchOptions
{
    sharedApplication->createWindow();

    return YES;
}

-(BOOL)application:(__unused UIApplication*)application didFinishLaunchingWithOptions:(__unused NSDictionary*)launchOptions
{
    return YES;
}

-(void)applicationDidBecomeActive:(__unused UIApplication*)application
{
}

-(void)applicationWillResignActive:(__unused UIApplication*)application
{
}

-(void)applicationDidEnterBackground:(__unused UIApplication*)application
{
}

-(void)applicationWillEnterForeground:(__unused UIApplication*)application
{
}

-(void)applicationWillTerminate:(__unused UIApplication*)application
{
}

-(void)applicationDidReceiveMemoryWarning:(__unused UIApplication*)application
{
}

@end

@interface ViewController: UIViewController
{
    demo::ApplicationIOS* application;
}

@end

@implementation ViewController

-(id)initWithWindow:(demo::ApplicationIOS*)initApplication
{
    if (self = [super init])
        application = initApplication;

    return self;
}

-(BOOL)prefersStatusBarHidden
{
    return YES;
}

-(void)textFieldDidChange:(__unused id)sender
{
}

-(NSUInteger)supportedInterfaceOrientations
{
    // TODO: add setting to limit orientations
    return UIInterfaceOrientationMaskAll;
}

-(void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];

    application->didResize(size.width, size.height);
}

-(void)deviceOrientationDidChange:(NSNotification*)note
{
    if (self.view)
    {
        CGSize size = self.view.frame.size;

        application->didResize(size.width, size.height);
    }
}

-(void)draw:(__unused CADisplayLink*)sender
{
    application->draw();
}

@end

namespace demo
{
    ApplicationIOS::ApplicationIOS()
    {
        sharedApplication = this;
        pool = [[NSAutoreleasePool alloc] init];
    }

    ApplicationIOS::~ApplicationIOS()
    {
        if (pool) [pool release];
    }

    void ApplicationIOS::createWindow()
    {
        screen = [UIScreen mainScreen];

        window = [[UIWindow alloc] initWithFrame:[screen bounds]];

        viewController = [[[ViewController alloc] initWithWindow:this] autorelease];
        window.rootViewController = viewController;

        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
        [[NSNotificationCenter defaultCenter] addObserver:viewController selector:@selector(deviceOrientationDidChange:)
                                                     name:UIDeviceOrientationDidChangeNotification
                                                   object:[UIDevice currentDevice]];

        CGRect windowFrame = [window bounds];

        width = static_cast<uint32_t>(windowFrame.size.width * screen.scale);
        height = static_cast<uint32_t>(windowFrame.size.height * screen.scale);

        view = [[UIView alloc] initWithFrame:windowFrame];
        viewController.view = view;

        [window makeKeyAndVisible];
    }

    void ApplicationIOS::draw()
    {
        render();
    }

    void ApplicationIOS::didResize(CGFloat newWidth, CGFloat newHeight)
    {
        width = static_cast<uint32_t>(newWidth * screen.scale);
        height = static_cast<uint32_t>(newHeight * screen.scale);

        onResize();
    }

    void ApplicationIOS::run()
    {
        UIApplicationMain(0, nil, nil, NSStringFromClass([AppDelegate class]));
    }

    std::string Application::getResourcePath()
    {
        CFBundleRef bundle = CFBundleGetMainBundle(); // [NSBundle mainBundle]
        CFURLRef path = CFBundleCopyResourcesDirectoryURL(bundle); // [bundle resourceURL]

        if (path)
        {
            char resourceDirectory[1024];
            CFURLGetFileSystemRepresentation(path, TRUE, reinterpret_cast<UInt8*>(resourceDirectory), sizeof(resourceDirectory));
            CFRelease(path);
            return resourceDirectory;
        }
        else
            throw std::runtime_error("Failed to get current directory");

        return "";
    }
}

int main()
{
    try
    {
        demo::ApplicationIOS application;
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
