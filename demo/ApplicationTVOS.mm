//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>
#include "ApplicationTVOS.hpp"

demo:: ApplicationTVOS* sharedApplication;

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
    demo::ApplicationTVOS* application;
}

@end

@implementation ViewController

-(id)initWithWindow:(demo::ApplicationTVOS*)initApplication
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

@end

@interface Canvas: UIView
{
    demo::ApplicationTVOS* application;
}

@end

@implementation Canvas

-(id)initWithFrame:(CGRect)frameRect andApplication:(demo::ApplicationTVOS*)initApplication
{
    if (self = [super initWithFrame:frameRect])
    {
        application = initApplication;
    }

    return self;
}

-(void)drawRect:(CGRect)dirtyRect
{
    [super drawRect:dirtyRect];

    application->draw();
}

-(void)draw:(__unused NSTimer*)timer
{
    [self setNeedsDisplay];
}

@end

static const void* getBytePointer(void* info)
{
    sr::RenderTarget* renderTarget = static_cast<sr::RenderTarget*>(info);

    return renderTarget->getFrameBuffer().getData().data();
}

namespace demo
{
    ApplicationTVOS::ApplicationTVOS()
    {
        sharedApplication = this;
        pool = [[NSAutoreleasePool alloc] init];
    }

    ApplicationTVOS::~ApplicationTVOS()
    {
        CGDataProviderRelease(provider);
        CGColorSpaceRelease(colorSpace);

        if (timer) [timer release];
        if (content) [content release];
        if (window)
        {
            window.rootViewController = nil;
            [window release];
        }
        if (pool) [pool release];
    }

    void ApplicationTVOS::createWindow()
    {
        screen = [UIScreen mainScreen];

        window = [[UIWindow alloc] initWithFrame:[screen bounds]];

        viewController = [[[ViewController alloc] initWithWindow:this] autorelease];
        window.rootViewController = viewController;

        CGRect windowFrame = [window bounds];

        width = static_cast<uint32_t>(windowFrame.size.width * screen.scale);
        height = static_cast<uint32_t>(windowFrame.size.height * screen.scale);

        content = [[Canvas alloc] initWithFrame:windowFrame andApplication:this];
        content.contentScaleFactor = screen.scale;
        viewController.view = content;

        componentsPerPixel = 4;
        bitsPerComponent = sizeof(uint8_t) * 8;

        CGDataProviderDirectCallbacks providerCallbacks = {
            0,
            getBytePointer,
            nullptr,
            nullptr,
            nullptr
        };

        colorSpace = CGColorSpaceCreateDeviceRGB();
        provider = CGDataProviderCreateDirect(&renderTarget, width * height * componentsPerPixel, &providerCallbacks);

        [window makeKeyAndVisible];

        timer = [[NSTimer scheduledTimerWithTimeInterval:0.016 target:content selector:@selector(draw:) userInfo:[NSValue valueWithPointer:this] repeats:YES] retain];

        setup();
    }

    void ApplicationTVOS::draw()
    {
        render();

        CGImageRef image = CGImageCreate(width, height, bitsPerComponent,
                                         bitsPerComponent * componentsPerPixel,
                                         componentsPerPixel * width,
                                         colorSpace,
                                         kCGBitmapByteOrderDefault | kCGImageAlphaLast,
                                         provider, nullptr, FALSE, kCGRenderingIntentDefault);

        CGContextRef context = UIGraphicsGetCurrentContext();

        CGContextDrawImage(context, [content frame], image);
        CGContextFlush(context);

        CGImageRelease(image);
    }

    void ApplicationTVOS::didResize(CGFloat newWidth, CGFloat newHeight)
    {
        width = static_cast<uint32_t>(newWidth * screen.scale);
        height = static_cast<uint32_t>(newHeight * screen.scale);

        CGDataProviderRelease(provider);

        CGDataProviderDirectCallbacks providerCallbacks = {
            0,
            getBytePointer,
            nullptr,
            nullptr,
            nullptr
        };

        provider = CGDataProviderCreateDirect(&renderTarget, width * height * componentsPerPixel, &providerCallbacks);
        
        onResize();
    }

    void ApplicationTVOS::run()
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
        demo::ApplicationTVOS application;
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
