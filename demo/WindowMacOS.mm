//
//  SoftwareRenderer
//

#include "WindowMacOS.hpp"
#include "Application.hpp"

@interface WindowDelegate: NSObject<NSWindowDelegate>
{
    WindowMacOS* window;
}

@end

@implementation WindowDelegate

-(id)initWithWindow:(WindowMacOS*)initWindow
{
    if (self = [super init])
    {
        window = initWindow;
    }

    return self;
}

@end

@interface Canvas: NSView
{
    NSInteger width;
    NSInteger height;
    size_t componentsPerPixel;
    size_t bitsPerComponent;
    CGColorSpaceRef colorSpace;
    CGDataProviderRef provider;
    //Bitmap bitmap;
    CGImageRef image;
}

@end

static const void* getBytePointer(void* info)
{
    /*Bitmap* bitmap = (Bitmap*)info;

    return bitmap->buffer;*/

    return nullptr;
}

static void freeBitmap(void* info)
{
    //Bitmap* bitmap = (Bitmap*)info;
    //free(bitmap->buffer);
}

@implementation Canvas

-(id)initWithFrame:(NSRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        width = frameRect.size.width;
        height = frameRect.size.height;
        componentsPerPixel = 4;
        bitsPerComponent = sizeof(uint8_t) * 8;

        CGDataProviderDirectCallbacks providerCallbacks = {
            0,
            getBytePointer,
            NULL,
            NULL,
            freeBitmap
        };

        /*bitmap.width = width;
        bitmap.height = height;
        bitmap.buffer = malloc(bitmap.width * bitmap.height * componentsPerPixel);

        colorSpace = CGColorSpaceCreateDeviceRGB();
        provider = CGDataProviderCreateDirect(&bitmap, width * height * componentsPerPixel, &providerCallbacks);*/
    }

    return self;
}

-(void)dealloc
{
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);

    [super dealloc];
}

-(void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];

    image = CGImageCreate(width, height, bitsPerComponent,
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

-(void)draw:(NSValue*)data
{

    [self setNeedsDisplay:TRUE];
}
@end

WindowMacOS::~WindowMacOS()
{
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


    content = [[Canvas alloc] initWithFrame:windowFrame];

    window.contentView = content;
    [window makeKeyAndOrderFront:nil];

    return 1;
}
