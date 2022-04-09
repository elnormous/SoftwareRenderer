//
//  SoftwareRenderer
//

#include <iostream>
#include <memory>
#include <stdexcept>
#include <CoreFoundation/CoreFoundation.h>
#include "ApplicationIOS.hpp"

demo::ApplicationIOS* sharedApplication;

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
        const CGSize size = self.view.frame.size;

        application->didResize(size.width, size.height);
    }
}

@end

@interface Canvas: UIView
{
    demo::ApplicationIOS* application;
}

@end

@implementation Canvas

-(id)initWithFrame:(CGRect)frameRect andApplication:(demo::ApplicationIOS*)initApplication
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
    const auto frameBuffer = static_cast<sr::Texture*>(info);
    return frameBuffer->getData().data();
}

namespace demo
{
    std::string getResourcePath()
    {
        CFBundleRef bundle = CFBundleGetMainBundle();
        if (!bundle) throw std::runtime_error{"Failed to get main bundle"};

        const cf::Pointer relativePath = CFBundleCopyResourcesDirectoryURL(bundle);
        if (!relativePath) throw std::runtime_error{"Failed to get current directory"};

        const cf::Pointer absolutePath = CFURLCopyAbsoluteURL(relativePath);
        if (!absolutePath) throw std::runtime_error{"Failed to copy absolute URL"};

        const cf::Pointer path = CFURLCopyFileSystemPath(absolutePath, kCFURLPOSIXPathStyle);
        if (!path) throw std::runtime_error{"Failed to copy file system path"};

        const auto maximumSize = CFStringGetMaximumSizeOfFileSystemRepresentation(path);
        const auto resourceDirectory = std::unique_ptr<char[]>(new char[static_cast<std::size_t>(maximumSize)]);
        if (!CFStringGetFileSystemRepresentation(path, resourceDirectory.get(), maximumSize))
            throw std::runtime_error{"Failed to get file system representation"};

        return std::string{resourceDirectory.get()};
    }

    ApplicationIOS::ApplicationIOS()
    {
        sharedApplication = this;
        pool = [[NSAutoreleasePool alloc] init];
    }

    ApplicationIOS::~ApplicationIOS()
    {
        if (content) [content release];
        if (window)
        {
            window.rootViewController = nil;
            [window release];
        }

        if (viewController) [viewController release];
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

        const CGRect windowFrame = [window bounds];

        const auto w = static_cast<std::size_t>(windowFrame.size.width * screen.scale);
        const auto h = static_cast<std::size_t>(windowFrame.size.height * screen.scale);

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
        provider = CGDataProviderCreateDirect(&getFrameBuffer(), w * h * componentsPerPixel, &providerCallbacks);

        [window makeKeyAndVisible];

        timer = [[NSTimer scheduledTimerWithTimeInterval:0.016
                                                  target:content
                                                selector:@selector(draw:)
                                                userInfo:[NSValue valueWithPointer:this]
                                                 repeats:YES] retain];

        setup(w, h);
    }

    void ApplicationIOS::draw()
    {
        render();

        const auto& frameBuffer = getFrameBuffer();
        
        CGImageRef image = CGImageCreate(frameBuffer.getWidth(), frameBuffer.getHeight(),
                                         bitsPerComponent,
                                         bitsPerComponent * componentsPerPixel,
                                         componentsPerPixel * frameBuffer.getWidth(),
                                         colorSpace,
                                         kCGBitmapByteOrder32Big | kCGImageAlphaNoneSkipLast,
                                         provider, nullptr, FALSE, kCGRenderingIntentDefault);

        CGContextRef context = UIGraphicsGetCurrentContext();

        CGContextDrawImage(context, [content frame], image);
        CGContextFlush(context);

        CGImageRelease(image);
    }

    void ApplicationIOS::didResize(CGFloat newWidth, CGFloat newHeight)
    {
        CGDataProviderDirectCallbacks providerCallbacks = {
            0,
            getBytePointer,
            nullptr,
            nullptr,
            nullptr
        };

        const auto w = static_cast<std::size_t>(newWidth * screen.scale);
        const auto h = static_cast<std::size_t>(newHeight * screen.scale);
        
        provider = CGDataProviderCreateDirect(&getFrameBuffer(),
                                              w * h * componentsPerPixel,
                                              &providerCallbacks);
        
        onResize(w, h);
    }

    void ApplicationIOS::run(int argc, char* argv[])
    {
        UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}

int main(int argc, char* argv[])
{
    try
    {
        demo::ApplicationIOS application;
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
