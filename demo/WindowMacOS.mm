//
//  SoftwareRenderer
//

#include "WindowMacOS.hpp"
#include "Application.hpp"

@interface WindowDelegate: NSObject<NSWindowDelegate>
{
    demo::WindowMacOS* window;
}

@end

@implementation WindowDelegate

-(id)initWithWindow:(demo::WindowMacOS*)initWindow
{
    if (self = [super init])
    {
        window = initWindow;
    }

    return self;
}

-(void)windowDidResize:(NSNotification*)notification
{
    window->didResize();
}

@end

@interface Canvas: NSView
{
    demo::WindowMacOS* window;
}

@end

@implementation Canvas

-(id)initWithFrame:(NSRect)frameRect andWindow:(demo::WindowMacOS*)initWindow
{
    if (self = [super initWithFrame:frameRect])
    {
        window = initWindow;
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

    window->draw();
}

-(void)draw:(NSTimer*)timer
{
    [self setNeedsDisplay:YES];
}

@end

static const void* getBytePointer(void* info)
{
    sr::Renderer* renderer = static_cast<sr::Renderer*>(info);
    const sr::Buffer& buffer = renderer->getFrameBuffer();

    return buffer.getData().data();
}

namespace demo
{
    WindowMacOS::WindowMacOS(Application& initApplication):
        Window(initApplication)
    {
    }

    WindowMacOS::~WindowMacOS()
    {
        CGDataProviderRelease(provider);
        CGColorSpaceRelease(colorSpace);

        [timer release];
        [content release];
        window.delegate = nil;
        [window release];
    }

    bool WindowMacOS::init(int argc, const char** argv)
    {
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
        provider = CGDataProviderCreateDirect(&renderer, width * height * componentsPerPixel, &providerCallbacks);

        window.contentView = content;
        [window makeKeyAndOrderFront:nil];

        [content setNeedsDisplay:TRUE];

        timer = [[NSTimer scheduledTimerWithTimeInterval:0.016 target:content selector:@selector(draw:) userInfo:[NSValue valueWithPointer:this] repeats:YES] retain];

        return Window::init(argc, argv);
    }

    void WindowMacOS::draw()
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

    void WindowMacOS::didResize()
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

        provider = CGDataProviderCreateDirect(&renderer, width * height * componentsPerPixel, &providerCallbacks);

        onResize();
    }
}
