#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ndk_DynamicActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "默认就是静态注册";
    return env->NewStringUTF(hello.c_str());
}