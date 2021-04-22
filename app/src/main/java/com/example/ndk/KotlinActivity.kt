package com.example.ndk

import android.os.Bundle
import android.util.Log
import android.view.View
import androidx.appcompat.app.AppCompatActivity

class KotlinActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_kotlin)
    }

    //  public native String stringFromJNIKotlin()
    external fun stringFromJNIKotlin(): String

    //  public native void sort(int[] array)
    external fun sort(arr: IntArray)

    //static{}
//    companion object {
//        init {
//            System.load("native-lib")
//        }
//    }
    var name1: String? = "T1"
    var name2: String? = "T2"
    var name3: String? = "T3"

    external fun initStaticCache()

    fun sortAction(view: View) {
        val arr = intArrayOf(12, 23, 15, 6, -4, 5)
        sort(arr)
        for (element in arr) {
            Log.e("Sort", " " + element)
        }
    }


}