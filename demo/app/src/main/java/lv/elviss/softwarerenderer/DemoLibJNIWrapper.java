//
//  SoftwareRenderer
//

package lv.elviss.softwarerenderer;

import android.graphics.Bitmap;

public class DemoLibJNIWrapper
{
    static
    {
        System.loadLibrary("demo");
    }

    public static native void init(int width, int height);
    public static native void onSizeChanged(int width, int height);
    public static native void onDraw(Bitmap bitmap);
}
