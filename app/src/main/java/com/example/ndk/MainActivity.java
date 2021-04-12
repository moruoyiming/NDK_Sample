package com.example.ndk;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    static final int A = 12;
    public String name = "Derry";
    public static int age = 29;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
        changeName();
        changeAge();
        callAddMethod();
        tv.setText(name + "age=" + age);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    //Java 本地方法 实现在Native层
    public native String getStringPWD();

    public static native String getStringPWD2();

    public native void changeName();

    public static native void changeAge();

    public  native void callAddMethod();


    //提供函数给native层
    public int add(int number1, int number2) {
        Log.i("sdfasf", (number1 + number2) + "");
        return number1 + number2;
    }
}

// 签名规则
//Java的boolean  --- Z
//Java的byte     --- B
//Java的char     --- C
//Java的short    --- S
//Java的int      --- I
//Java的long     --- J
//Java的float    --- F
//Java的double   --- D
//Java的void     --- V
//Java的引用类型   --- Lxxx/xxx/xx/类名
//Java的String   --- Ljava/lang/String
//Java的array    --- int[] ----> [I    double[][][] ----> [[[D

//javap -s -p xxx.class
// -s输出xxx.class的所有属性和方法的签名
// -p忽略私有公开的所有属性方法全部输出