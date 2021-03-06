//
//  SoftwareRenderer
//

#include <iostream>
#include <memory>
#include <stdexcept>
#include <CoreFoundation/CoreFoundation.h>
#include "ApplicationTVOS.hpp"

demo::ApplicationTVOS* sharedApplication;

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
    return UIInterfaceOrientationMaskAll;
}

-(void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];

    application->didResize(size.width, size.height);
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

        const CGRect windowFrame = [window bounds];

        width = static_cast<std::size_t>(windowFrame.size.width * screen.scale);
        height = static_cast<std::size_t>(windowFrame.size.height * screen.scale);

        content = [[Canvas alloc] initWithFrame:windowFrame andApplication:this];
        content.contentScaleFactor = screen.scale;
        viewController.view = content;

        componentsPerPixel = 4;
        bitsPerComponent = sizeof(std::uint8_t) * 8;

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
                                         kCGBitmapByteOrder32Big | kCGImageAlphaNoneSkipLast,
                                         provider, nullptr, FALSE, kCGRenderingIntentDefault);

        CGContextRef context = UIGraphicsGetCurrentContext();

        CGContextDrawImage(context, [content frame], image);
        CGContextFlush(context);

        CGImageRelease(image);
    }

    void ApplicationTVOS::didResize(CGFloat newWidth, CGFloat newHeight)
    {
        width = static_cast<std::size_t>(newWidth * screen.scale);
        height = static_cast<std::size_t>(newHeight * screen.scale);

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

    void ApplicationTVOS::run(int argc, char* argv[])
    {
        UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }

    std::string Application::getResourcePath()
    {
        CFBundleRef bundle = CFBundleGetMainBundle();
        CFURLRef relativePath = CFBundleCopyResourcesDirectoryURL(bundle);

        if (relativePath)
        {
            CFURLRef absolutePath = CFURLCopyAbsoluteURL(relativePath);
            if (absolutePath)
            {
                CFStringRef path = CFURLCopyFileSystemPath(absolutePath, kCFURLPOSIXPathStyle);
                if (path)
                {
                    const auto maximumSize = CFStringGetMaximumSizeOfFileSystemRepresentation(path);
                    auto resourceDirectory = std::unique_ptr<char[]>(new char[static_cast<std::size_t>(maximumSize)]);
                    CFStringGetFileSystemRepresentation(path, resourceDirectory.get(), maximumSize);
                    CFRelease(path);
                    return std::string(resourceDirectory.get());
                }
                CFRelease(absolutePath);
            }
            CFRelease(relativePath);
            return std::string();
        }
        else
            throw std::runtime_error("Failed to get current directory");

        return "";
    }
}

int main(int argc, char* argv[])
{
    try
    {
        demo::ApplicationTVOS application;
        application.run(argc, argv);

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
