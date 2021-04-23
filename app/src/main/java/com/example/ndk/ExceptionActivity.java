package com.example.ndk;

import android.os.Bundle;
import android.util.Log;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class ExceptionActivity extends AppCompatActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_exception);
    }

    static String name1 = "T1";

    public static native void exeption();

    public static native void exeption2() throws NoSuchFieldException;//接收C++层抛出的异常

    public static native void exeption3();

    public void exceptionAction(View view) {
//        exeption();//

//        try {
//            exeption2();
//        } catch (Exception e) {
//            e.printStackTrace();
//            Log.i("sfdfs", e.toString());
//        }

        exeption3();
    }

    //封装函数 提供给native调用
    public static void show() throws Exception{
        Log.i("ExceptionActivity"," Java层抛出异常 ");
        throw new NullPointerException("Java层抛出异常");
    }


}
