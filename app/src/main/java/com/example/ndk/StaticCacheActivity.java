package com.example.ndk;

import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class StaticCacheActivity extends AppCompatActivity {

    public static String name1 = "张三1";
    public static String name2 = "张三2";
    public static String name3 = "张三3";

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_staticcache);
    }

    //构造代码块 先执行初始化
//    {
//        initStaticCache();
//    }

    public static native void localCache(String name);//演示弊端，普通的局部缓存

    public static native void initStaticCache();//初始化静态缓存

    public static native void useStaticCache(String name);

    public static native void clearStaticCache();//清除静态缓存


    public void staticCacheAction(View view) {
//        localCache(name1);
//        localCache(name2);
//        localCache(name3);
        initStaticCache();
        useStaticCache(name1);
        useStaticCache(name2);
        useStaticCache(name3);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        clearStaticCache();
    }
}
