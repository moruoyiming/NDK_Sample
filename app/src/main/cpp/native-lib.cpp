#include <jni.h>
#include <string>
//JNI打印，NDK工具链里的liblog.so库
#include <android/log.h>

#define TAG "JNILOG"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR


extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ndk_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

//必须采用c的编译方式，请看JNIEnv内部源码
extern "C"
JNIEXPORT //标记该方法可以被外部调用
jstring //Java 与 Natvie 转换使用
JNICALL //关键字 JNI标记

//JNIEnv * env JNI:的桥梁环境

//jobject thiz 谁调用该方法，该对象就是谁的实例
//jclass clazz 谁调用该方法，该class就是谁的class
com_example_ndk_MainActivity_getStringPWD(JNIEnv *env, jobject thiz) {
    //无论是C还是C++ 最终是调用到C的JNINativeInterface，所以必须采用C的方式extern "C"


//静态函数
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ndk_MainActivity_getStringPWD2(JNIEnv *env, jclass clazz) {
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_MainActivity_changeName(JNIEnv *env, jobject thiz) {

    //获取class
    jclass j_cls = env->GetObjectClass(thiz);
    //获取属性
    // GetFieldID(jclass clazz, const char* name, const char* sig)
    // class 属性名 属性签名
    jfieldID j_fid = env->GetFieldID(j_cls, "name", "Ljava/lang/String;");

    //转换工作
    jstring j_str = static_cast<jstring>(env->GetObjectField(thiz, j_fid));

    //打印字符串
    char *c_str = const_cast<char * >(env->GetStringUTFChars(j_str, NULL));
    LOGD("native :%s", c_str);
    jstring jName = env->NewStringUTF("Beyond");
    env->SetObjectField(thiz, j_fid, jName);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_MainActivity_changeAge(JNIEnv *env, jclass clazz) {
    // TODO: implement changeAge()
    jfieldID j_fid = env->GetStaticFieldID(clazz, "age", "I");

    jint age = env->GetStaticIntField(clazz, j_fid);

    age += 1;
    env->SetStaticIntField(clazz, j_fid, age);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_MainActivity_callAddMethod(JNIEnv *env, jobject clazz) {
    //调用java层add函数
    //获取class
    jclass j_cls = env->GetObjectClass(clazz);
    jmethodID j_mid = env->GetMethodID(j_cls, "add", "(II)I");
    //调用java函数
    jint sum = env->CallIntMethod(clazz, j_mid, 3, 3);
    LOGD("sum result:%d",sum);
}