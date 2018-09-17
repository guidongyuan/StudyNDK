package com.guidongyuan.studyndk;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    public int age = 18;
    public String name = "yuan";
    public static String school = "GDUT";

    private static final String TAG = "StudyNDK";

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());

        Log.i(TAG, "stringFromJNI() = "+stringFromJNI());

        Log.i(TAG, "passValueToJNI() = "+passValueToJNI(100, "passToJNI"));

        int[] intArrays = new int[]{0, 1, 2};
        String[] strArrays = new String[]{"zero","first","second"};
        Log.i(TAG, "passArrayToJNI() = "+passArrayToJNI(intArrays, strArrays));
        // 验证在JNI的修改是否有效
        for (int i = 0;i < intArrays.length;i++){
            Log.i(TAG, "修改后 int数据为：i = " + intArrays[i]);
        }

        Log.i(TAG, "passBeanToJNI = "+passBeanToJNI(new Bean()));

        callJNI();
        Log.i(TAG, "修改后 age = " +age + ",name = "+name + ",school = "+school);

        createThread();

        dynamicRegister();
        Log.i(TAG, "dynamicRegisteStatic = " + dynamicRegisterStatic(100));
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    /**
     * 传递参数给JNI
     * @param intValue
     * @param strValue
     * @return
     */
    public native String passValueToJNI(int intValue,String strValue);

    /**
     * 传递数组给JNI
     * @param intArray
     * @param strArray
     * @return
     */
    public native String passArrayToJNI(int[] intArray, String strArray[]);

    /**
     * 传递自定义数据类型
     * @param bean
     * @return
     */
    public native String passBeanToJNI(Bean bean);

    public native void callJNI();

    public void callFromJNI(){
        Log.i(TAG, "callFromJNI");
    }

    public void callFromJNI(int i) {
        Log.i(TAG, "callFromJNI i = "+i);
    }

    public void callFromJNI(String string) {
        Log.i(TAG, "callFromJNI string = "+string);
    }

    public static void callStaticFromJNI(){
        Log.i(TAG, "callStaticFromJNI");
    }

    /**
     * 在native中创建线程
     */
    public native void createThread();

    /**
     * native中的线程回调
     */
    public void updateUI(){
        Log.i(TAG, "updateUI");
    }

    public native void dynamicRegister();

    public native static String dynamicRegisterStatic(int i);
}
