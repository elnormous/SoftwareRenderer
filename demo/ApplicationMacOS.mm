//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>
#include "ApplicationMacOS.hpp"

@interface AppDelegate: NSObject<NSApplicationDelegate>
{
    demo::Application* application;
}
@end

@implementation AppDelegate

-(id)initWithApplication:(demo::Application*)initApplication
{
    if (self = [super init])
        application = initApplication;

    return self;
}

-(void)applicationWillFinishLaunching:(__unused NSNotification*)notification
{
}

-(void)applicationDidFinishLaunching:(__unused NSNotification*)notification
{
}

-(void)applicationWillTerminate:(__unused NSNotification*)notification
{
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(__unused NSApplication*)sender
{
    return YES;
}

-(void)applicationDidBecomeActive:(__unused NSNotification*)notification
{
}

-(void)applicationDidResignActive:(__unused NSNotification*)notification
{
}

-(void)handleQuit:(__unused id)sender
{
    [[NSApplication sharedApplication] terminate:nil];
}

@end

@interface WindowDelegate: NSObject<NSWindowDelegate>
{
    demo::ApplicationMacOS* application;
}

@end

@implementation WindowDelegate

-(id)initWithWindow:(demo::ApplicationMacOS*)initApplication
{
    if (self = [super init])
    {
        application = initApplication;
    }

    return self;
}

-(void)windowDidResize:(__unused NSNotification*)notification
{
    application->didResize();
}

@end

@interface Canvas: NSView
{
    demo::ApplicationMacOS* application;
}

@end

@implementation Canvas

-(id)initWithFrame:(NSRect)frameRect andWindow:(demo::ApplicationMacOS*)initApplication
{
    if (self = [super initWithFrame:frameRect])
    {
        application = initApplication;
    }

    return self;
}

-(void)dealloc
{
    [super dealloc];
}

-(void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];
}

-(void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];

    application->draw();
}

-(void)draw:(__unused NSTimer*)timer
{
    [self setNeedsDisplay:YES];
}

@end

static const void* getBytePointer(void* info)
{
    sr::RenderTarget* renderTarget = static_cast<sr::RenderTarget*>(info);

    return renderTarget->getFrameBuffer().getData().data();
}

namespace demo
{
    ApplicationMacOS::ApplicationMacOS()
    {
    }

    ApplicationMacOS::~ApplicationMacOS()
    {
        CGDataProviderRelease(provider);
        CGColorSpaceRelease(colorSpace);

        [timer release];
        [content release];
        window.delegate = nil;
        [window release];
    }

    void ApplicationMacOS::draw()
    {
        render();

        CGImageRef image = CGImageCreate(width, height, bitsPerComponent,
                                         bitsPerComponent * componentsPerPixel,
                                         componentsPerPixel * width,
                                         colorSpace,
                                         kCGBitmapByteOrderDefault | kCGImageAlphaLast,
                                         provider, NULL, FALSE, kCGRenderingIntentDefault);

        CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];

        CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);
        CGContextFlush(context);

        CGImageRelease(image);
    }

    void ApplicationMacOS::didResize()
    {
        NSRect windowFrame = [NSWindow contentRectForFrameRect:[window frame]
                                                     styleMask:[window styleMask]];

        width = static_cast<uint32_t>(windowFrame.size.width);
        height = static_cast<uint32_t>(windowFrame.size.height);

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

    void ApplicationMacOS::run()
    {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

        NSApplication* sharedApplication = [NSApplication sharedApplication];
        [sharedApplication activateIgnoringOtherApps:YES];
        [sharedApplication setDelegate:[[[AppDelegate alloc] initWithApplication:this] autorelease]];

        NSMenu* mainMenu = [[[NSMenu alloc] initWithTitle:@"Main Menu"] autorelease];

        NSMenuItem* mainMenuItem = [[[NSMenuItem alloc] init] autorelease];
        [mainMenu addItem:mainMenuItem];

        NSMenu* subMenu = [[[NSMenu alloc] init] autorelease];
        [mainMenuItem setSubmenu:subMenu];

        NSMenuItem* quitItem = [[[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(handleQuit:) keyEquivalent:@"q"] autorelease];
        [quitItem setTarget: [sharedApplication delegate]];
        [subMenu addItem:quitItem];

        sharedApplication.mainMenu = mainMenu;

        // create window
        screen = [NSScreen mainScreen];

        CGSize windowSize;
        windowSize.width = round(screen.frame.size.width * 0.6);
        windowSize.height = round(screen.frame.size.height * 0.6);

        NSRect frame = NSMakeRect(round(screen.frame.size.width / 2.0f - windowSize.width / 2.0f),
                                  round(screen.frame.size.height / 2.0f - windowSize.height / 2.0f),
                                  windowSize.width, windowSize.height);

        NSWindowStyleMask windowStyleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;

        window  = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:windowStyleMask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO
                                                 screen:screen];
        [window setReleasedWhenClosed:NO];

        window.acceptsMouseMovedEvents = YES;
        windowDelegate = [[WindowDelegate alloc] initWithWindow:this];
        window.delegate = windowDelegate;

        [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
        [window setTitle:@"SoftwareRenderer"];

        NSRect windowFrame = [NSWindow contentRectForFrameRect:[window frame]
                                                     styleMask:[window styleMask]];

        width = static_cast<uint32_t>(windowFrame.size.width);
        height = static_cast<uint32_t>(windowFrame.size.height);

        content = [[Canvas alloc] initWithFrame:windowFrame andWindow:this];

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

        window.contentView = content;
        [window makeKeyAndOrderFront:nil];

        [content setNeedsDisplay:TRUE];

        timer = [[NSTimer scheduledTimerWithTimeInterval:0.016 target:content selector:@selector(draw:) userInfo:[NSValue valueWithPointer:this] repeats:YES] retain];

        setup();

        [sharedApplication run];

        [pool release];
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
        demo::ApplicationMacOS application;
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
