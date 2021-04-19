package com.example.ndk;

import android.app.Activity;
import android.os.Bundle;

import androidx.annotation.Nullable;

public class DynamicActivity extends Activity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dynamic);
    }

    //第一部分 动态注册

    public native String stringFromJNI();//默认生成的，起始也是静态注册方式

    public native void staticRigster();//静态注册

    public native void dynamicRigster();//动态注册

    public native int dynamicRigster2(String valueStr);//动态注册

    //第二部分 JNI线程区域

    public native void nativeThread();//Java层调用Native层的函数，完成JNI线程

    //第三部分

    public native void nativeFun1();

    public native void nativeFun2();

    public static native void staticFun3();

    public static native void staticFun4();




}
