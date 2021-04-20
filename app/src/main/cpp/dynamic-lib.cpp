#include <jni.h>
#include <string>
#include <android/log.h>
#include <pthread.h>//在AS上 pthread 不需额外配置

#define TAG "JNILOG"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)


extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ndk_DynamicActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "默认就是静态注册";
    return env->NewStringUTF(hello.c_str());
}


//默认情况下 ，就是静态注册，静态注册是最简单的方式。
//静态注册：缺点 1.JNI函数名非常长
//              2.捆绑上层包名+类名
//              3.调用函数时，才会匹配JNI函数。性能上低于动态注册
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_DynamicActivity_staticRegister(JNIEnv *env, jobject thiz) {
    // TODO: implement staticRegister()
}

JavaVM *mJavaVm = nullptr; //0x121212 系统乱值。nullptr C++11后，取代NULL，作用是可以初始化指针赋值。

const char *className = "com/example/ndk/DynamicActivity";

//Java层的方法
const char *javaMethod1 = "dynamicRegister";

//JNI层的方法
// native 真正的函数
// void dynamicMethod01(JNIEnv *env, jobject thiz) { // OK的
void dynamicMethod(JNIEnv *env, jobject thiz) {// 也OK  如果你用不到  JNIEnv jobject ，可以不用写
    LOGD("我是动态注册的函数 dynamicMethod...");
}


int dynamicMethod2(JNIEnv *env, jobject thiz, jstring valueStr) {
    const char *text = env->GetStringUTFChars(valueStr, nullptr);
    LOGD("我是动态注册的函数 dynamicMethod02... %s", text);
    env->ReleaseStringUTFChars(valueStr, text);
    return 200;
}


/*
    typedef struct {
    const char* name;       // 函数名
    const char* signature; // 函数的签名
    void*       fnPtr;     // 函数指针
 } JNINativeMethod;
 */
static const JNINativeMethod jniNativeMethod[] = {
        {javaMethod1,        "()V",                   (void *) (dynamicMethod)},
        {"dynamicRegister2", "(Ljava/lang/String;)I", (int *) (dynamicMethod2)}
};

// 动态注册代码
// JAVA:像Java的构造函数。如果不写构造函数，默认就有构造函数。如果写了构造函数，覆写默认的构造函数。
// JNI:JNI_OnLoad函数。如果不写JNI_OnLoad函数，默认就有JNI_OnLoad函数。如果写了JNI_OnLoad函数，覆写默认的JNI_OnLoad函数。
JNIEXPORT jint JNI_OnLoad(JavaVM *javaVm, void *) {
    ::mJavaVm = javaVm;

    //做动态注册 全部做完
    JNIEnv *jniEnv = nullptr;
    int result = mJavaVm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);
    //result 等于0就是成功 【C库FFmpeg成功就是0】
    if (result != JNI_OK) {
        return -1;// -1崩溃
    }

    LOGE("System.loadLibrary ---》 JNI Load init");

    jclass mainActivityClass = jniEnv->FindClass(className);

    // jint RegisterNatives(Class, 我们的数组==jniNativeMethod， 注册的数量 = 2)
    jniEnv->RegisterNatives(mainActivityClass,
                            jniNativeMethod,
                            sizeof(jniNativeMethod) / sizeof(JNINativeMethod));

    LOGE("动态 注册没有毛病");

    return JNI_VERSION_1_6;//AS的JDK在JNI默认最高1.6      存Java的JDKJNI 1.8
}

//***********************************************线程************************************************************
class MyContext {
public:
    JNIEnv *jniEnv = nullptr;  // 不能跨线程 ，会奔溃
    jobject instance = nullptr; // 不能跨线程 ，会奔溃
};

void *myThreadTaskAction(void *pVoid) {// 当前是异步线程
    LOGE("myThreadTaskAction run");

    // 需求：有这样的场景，例如：下载完成 ，下载失败，等等，必须告诉Activity UI端，所以需要在子线程调用UI端

    // 这两个是必须要的
    // JNIEnv *env
    // jobject thiz   OK

    MyContext *myContext = static_cast<MyContext *>(pVoid);

    // jclass mainActivityClass = myContext->jniEnv->FindClass(mainActivityClassName); // 不能跨线程 ，会奔溃
    // mainActivityClass = myContext->jniEnv->GetObjectClass(myContext->instance); // 不能跨线程 ，会奔溃

    // TODO 解决方式 （安卓进程只有一个 JavaVM，是全局的，是可以跨越线程的）
    JNIEnv *jniEnv = nullptr; // 全新的JNIEnv  异步线程里面操作
    jint attachResult = ::mJavaVm->AttachCurrentThread(&jniEnv,
                                                       nullptr); // 附加当前异步线程后，会得到一个全新的 env，此env相当于是子线程专用env
    if (attachResult != JNI_OK) {
        return 0; // 附加失败，返回了
    }

    // 1.拿到class
    jclass mainActivityClass = jniEnv->GetObjectClass(myContext->instance);

    // 2.拿到方法
    jmethodID updateActivityUI = jniEnv->GetMethodID(mainActivityClass, "updateActivityUI", "()V");

    // 3.调用
    jniEnv->CallVoidMethod(myContext->instance, updateActivityUI);

    ::mJavaVm->DetachCurrentThread(); // 必须解除附加，否则报错

    LOGE("C++ 异步线程OK");

    return nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_DynamicActivity_nativeThread(JNIEnv *env, jobject thiz) {
    /*  pthread_t pid;
      pthread_create(&pid, nullptr, myThreadTaskAction, nullptr);
      pthread_join(pid, nullptr);*/
    MyContext *myContext = new MyContext;
    myContext->jniEnv = env;
//  myContext->instance = thiz;//默认是局部引用，会崩溃
    myContext->instance = env->NewGlobalRef(thiz);//提升全局引用
    pthread_t pid;
    pthread_create(&pid, nullptr, myThreadTaskAction, myContext);
    pthread_join(pid, nullptr);

}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_DynamicActivity_closeThread(JNIEnv *env, jobject thiz) {
    // 做释放工作
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_DynamicActivity_nativeFun1(JNIEnv *env, jobject thiz) {
    JavaVM *javaVm = nullptr;
    env->GetJavaVM(&javaVm);

    // 打印：当前函数env地址， 当前函数jvm地址， 当前函数job地址,  JNI_OnLoad的jvm地址
    LOGE("nativeFun1 当前函数env地址%p,  当前函数jvm地址:%p,  当前函数job地址:%p, JNI_OnLoad的jvm地址:%p\n", env, javaVm,
         thiz, ::mJavaVm);
}

void * run(void *){//native的自线程 env 地址 和 Java的自线程env地址，一样吗？不一样的
    JNIEnv * newEnv = nullptr;
    ::mJavaVm->AttachCurrentThread(&newEnv, nullptr);
    // 打印：当前函数env地址， 当前函数jvm地址， 当前函数clazz地址,  JNI_OnLoad的jvm地址

    LOGE("run jvm地址:%p,  当前run函数的newEnv地址:%p \n", ::mJavaVm, newEnv);

    ::mJavaVm->DetachCurrentThread();

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_DynamicActivity_nativeFun2(JNIEnv *env, jobject thiz) {
    JavaVM *javaVm = nullptr;
    env->GetJavaVM(&javaVm);

    // 打印：当前函数env地址， 当前函数jvm地址， 当前函数job地址,  JNI_OnLoad的jvm地址
    LOGE("nativeFun2 当前函数env地址%p,  当前函数jvm地址:%p,  当前函数job地址:%p, JNI_OnLoad的jvm地址:%p\n", env, javaVm,
         thiz, ::mJavaVm);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_DynamicActivity_staticFun3(JNIEnv *env, jclass clazz) {
    JavaVM * javaVm = nullptr;
    env->GetJavaVM(&javaVm);

    // 打印：当前函数env地址， 当前函数jvm地址， 当前函数clazz地址,  JNI_OnLoad的jvm地址
    LOGE("nativeFun3 当前函数env地址%p,  当前函数jvm地址:%p,  当前函数clazz地址:%p, JNI_OnLoad的jvm地址:%p\n", env, javaVm, clazz, ::mJavaVm);

    // 调用run
    pthread_t pid;
    pthread_create(&pid, nullptr, run, nullptr);
}

// Java子线程调用的
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_DynamicActivity_staticFun4(JNIEnv *env, jclass clazz) {
    JavaVM * javaVm = nullptr;
    env->GetJavaVM(&javaVm);

    // 打印：当前函数env地址， 当前函数jvm地址， 当前函数clazz地址,  JNI_OnLoad的jvm地址
    LOGE("nativeFun4 当前函数env地址%p,  当前函数jvm地址:%p,  当前函数clazz地址:%p, JNI_OnLoad的jvm地址:%p\n", env, javaVm, clazz, ::mJavaVm);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_derry_as_1jni_1project_MainActivity2_nativeFun5(JNIEnv *env, jobject job) {
    JavaVM * javaVm = nullptr;
    env->GetJavaVM(&javaVm);

    // 打印：当前函数env地址， 当前函数jvm地址， 当前函数clazz地址,  JNI_OnLoad的jvm地址
    LOGE("nativeFun5 当前函数env地址%p,  当前函数jvm地址:%p,  当前函数job地址:%p, JNI_OnLoad的jvm地址:%p\n", env, javaVm, job, ::mJavaVm);
}

// TODO 纠结细节的结论：
// 1. JavaVM全局，绑定当前进程， 只有一个地址
// 2. JNIEnv线程绑定， 绑定主线程，绑定子线程
// 3. jobject 谁调用JNI函数，谁的实例会给jobject

// JNIEnv *env 不能跨越线程，否则奔溃，  他可以跨越函数 【解决方式：使用全局的JavaVM附加当前异步线程 得到权限env操作】
// jobject thiz 不能跨越线程，否则奔溃，不能跨越函数，否则奔溃 【解决方式：默认是局部引用，提升全局引用，可解决此问题】
// JavaVM 能够跨越线程，能够跨越函数


// C++ Derry 主动增加了 6节课
// JNI Derry 主动增加了 4节课
// 基础牢，后面很快都更得上

// 2节课 JNI 结束了

// Linux服务器 上一期 之前是讲了一节课
// Derry 主动增加了 4节课，Linux 编译FFmpeg 详情，Linux运维工程师语法的 来龙去脉

// Cmake MK

// 音视频 领域

// 视图图像 领域

// Derry 给大家增加的 智能家居 串口编程 ....

// 开发版 主板 USB 电脑 串口编程 JNI NDK  淘宝上