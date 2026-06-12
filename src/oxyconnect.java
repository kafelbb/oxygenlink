import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.drawable.Drawable;
import android.os.Looper;
import java.io.ByteArrayOutputStream;
import java.lang.reflect.Method;

public class oxyconnect {
    public static void main(String[] args) {
        if (args.length == 0) {
            System.err.println("Usage: app_process ... oxyconnect <package_name>");
            System.exit(1);
        }
        
        String packageName = args[0];
        
        try {
            if (Looper.myLooper() == null) {
                Looper.prepareMainLooper();
            }

            Class<?> activityThreadClass = Class.forName("android.app.ActivityThread");
            Method systemMainMethod = activityThreadClass.getDeclaredMethod("systemMain");
            systemMainMethod.setAccessible(true);
            Object activityThread = systemMainMethod.invoke(null);
            
            Method getSystemContextMethod = activityThreadClass.getDeclaredMethod("getSystemContext");
            getSystemContextMethod.setAccessible(true);
            Context context = (Context) getSystemContextMethod.invoke(activityThread);
            
            if (context == null) {
                System.err.println("Error: Context is null");
                System.exit(1);
            }
            
            PackageManager pm = context.getPackageManager();
            ApplicationInfo appInfo = pm.getApplicationInfo(packageName, 0);
            Drawable icon = appInfo.loadIcon(pm);
            
            if (icon == null) {
                System.err.println("Error: Icon not found");
                System.exit(1);
            }

            Bitmap bitmap = Bitmap.createBitmap(64, 64, Bitmap.Config.ARGB_8888);
            Canvas canvas = new Canvas(bitmap);
            
            icon.setBounds(0, 0, 64, 64);
            icon.draw(canvas);
            
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            bitmap.compress(Bitmap.CompressFormat.PNG, 100, baos);
            
            System.out.write(baos.toByteArray());
            System.out.flush();
            
            System.exit(0);
        } catch (Exception e) {
            System.err.println("Java Error: " + e.getMessage());
            e.printStackTrace();
            System.exit(1);
        }
    }
}