//
//  SoftwareRenderer
//

package lv.elviss.softwarerenderer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.view.View;
import android.util.Log;

public class BitmapView extends View
{
    Bitmap bitmap;

    public BitmapView(Context context)
    {
        super(context);
    }

    @Override public void onSizeChanged(int w,  int h,  int oldw,  int oldh)
    {
        if (bitmap == null)
            DemoLibJNIWrapper.init(w, h);
        else
            DemoLibJNIWrapper.onSizeChanged(w, h);

        bitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
    }

    @Override public void onDraw(Canvas canvas)
    {
        DemoLibJNIWrapper.onDraw(bitmap);

        canvas.drawBitmap(bitmap, new Rect(0, 0, getWidth(), getHeight()), new Rect(0, 0, 100, 100), null);
    }
}
