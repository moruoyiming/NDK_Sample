#include <jni.h>
#include <string>
//JNI打印，NDK工具链里的liblog.so库
#include <android/log.h>

#define TAG "JNILOG"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)


extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ndk_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

//必须采用c的编译方式，请看JNIEnv内部源码
//JNIEXPORT 标记该方法可以被外部调用
//jstring Java 与 Natvie 转换使用
//关键字 JNI标记
//jobject thiz 谁调用该方法，该对象就是谁的实例
//jclass clazz 谁调用该方法，该class就是谁的class
//JNIEnv * env JNI:的桥梁环境
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ndk_MainActivity_getStringPWD(JNIEnv *env, jobject thiz) {
    //无论是C还是C++ 最终是调用到C的JNINativeInterface，所以必须采用C的方式extern "C"
    //静态函数
}

extern "C"
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
    LOGD("sum result:%d", sum);
}
//jint = int
//jstring = String
//jintArray = int[]
//jobjectArray = 引用类型对象[]
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_MainActivity_testArrayAction(JNIEnv *env, jobject thiz,
                                                  jint count,
                                                  jstring text_info,
                                                  jintArray ints,
                                                  jobjectArray strs) {
    //1.基本数据类型 jint count
    int countInt = count;
    LOGD("参数一 countInt:%d", countInt);
    const char *textInfo = env->GetStringUTFChars(text_info, NULL);
    LOGD("参数二 textInfo:%s", textInfo);

    //2.int数组转成 int*
    int *jintArray = env->GetIntArrayElements(ints, NULL);
    jsize size = env->GetArrayLength(ints);
    for (int i = 0; i < size; ++i) {
        *(jintArray + i) += 100;
        LOGD("参数三 int[]:%d", *jintArray + i);
    }

    //JNIEnv *env == 操纵杆 JNI层
    // 0:刷新Java数组 并释放C++层数组
    // JNI_COMMIT：只提交 只刷新Java数组，不释放C++层数组
    // JNI_ABORT: 只释放C++层数组
    env->ReleaseIntArrayElements(ints, jintArray, 0);


    //3. jobjectArray 代表是Java的引用类型数组
    jsize jsize1 = env->GetArrayLength(strs);
    for (int i = 0; i < jsize1; ++i) {
        jstring jobj = static_cast<jstring>(env->GetObjectArrayElement(strs, i));
        const char *jobjCharp = env->GetStringUTFChars(jobj, NULL);
        LOGD("参数四 strs[]:%s", jobjCharp);
        //释放jstring
        env->ReleaseStringUTFChars(jobj, jobjCharp);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_MainActivity_putObject(JNIEnv *env, jobject thiz, jobject student,
                                            jstring str) {
    const char *strChar = env->GetStringUTFChars(str, NULL);
    env->ReleaseStringUTFChars(str, strChar);
//    1.寻找类
    jclass studentClass = env->GetObjectClass(student);
//    jclass studentClass = env->FindClass("com/example/ndk/bean/Student");
//    2.Student类里面的函数规则 签名
    jmethodID setName = env->GetMethodID(studentClass, "setName", "(Ljava/lang/String;)V");
    jmethodID getName = env->GetMethodID(studentClass, "getName", "()Ljava/lang/String;");
    jmethodID showInfo = env->GetStaticMethodID(studentClass, "showInfo", "(Ljava/lang/String;)V");
    jmethodID setAge = env->GetMethodID(studentClass, "setAge", "(I)V");
//    3.调用setName
    jstring value = env->NewStringUTF("张三");
    env->CallVoidMethod(student, setName, value);
//    4.调用getName
    jstring getNameResult = static_cast<jstring>(env->CallObjectMethod(student, getName));
    const char *getNameValue = env->GetStringUTFChars(getNameResult, NULL);
    LOGD("getNameResult:%s", getNameValue);
//    5.调用showInfo
    jstring info = env->NewStringUTF("北望气");
    env->CallStaticVoidMethod(studentClass, showInfo, info);
//    6.调用setAge
    jint intValue = 23;
    env->CallVoidMethod(student, setAge, intValue);

}


// 局部引用  全局引用
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_MainActivity_insertObject(JNIEnv *env, jobject thiz) {
    //1.通过包名+类名的方式 拿到Student class
    const char *studentstr = "com/example/ndk/bean/Student";
    jclass studentClass = env->FindClass(studentstr);
    //2.通过student的class 实例化此Student对象 C++ new
    jobject studentObj = env->AllocObject(studentClass);//AllocObject 只实例化对象，不会调用对象的构造函数
    //3.函数规则
    jmethodID setName = env->GetMethodID(studentClass, "setName", "(Ljava/lang/String;)V");
    jmethodID setAge = env->GetMethodID(studentClass, "setAge", "(I)V");
    jstring nameValue = env->NewStringUTF("东方不败");
    env->CallVoidMethod(studentObj, setName, nameValue);
    env->CallVoidMethod(studentObj, setAge, 30);
//    env->NewObject();//NewObject 实例化对象，会调用对象的构造函数

    const char *personstr = "com/example/ndk/bean/Person";
    jclass personClass = env->FindClass(personstr);
    jobject personObj = env->AllocObject(personClass);
    jmethodID setStudent = env->GetMethodID(personClass, "setStudent",
                                            "(Lcom/example/ndk/bean/Student;)V");
    env->CallVoidMethod(personObj, setStudent, studentObj);
    //规范 释放
    env->DeleteLocalRef(studentClass);
    env->DeleteLocalRef(personClass);
    env->DeleteLocalRef(studentObj);
    env->DeleteLocalRef(personObj);


    //TODO 局部引用 jobject jclass jstring  【函数弹栈后，会自动释放】
}

jclass dogClass;//这个是局部引用，不是全局引用


extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_MainActivity_testQuote(JNIEnv *env, jobject thiz) {
    if (NULL == dogClass) {
        const char *dogClassChar = "com/example/ndk/bean/Dog";
        //TODO 局部引用
//        dogClass = env->FindClass(dogClassChar);
        //TODO 升级全局引用， JNI函数结束 不释放dogClass引用。
        //类似 C++对象 new ，需要手动delete
        jclass temp = env->FindClass(dogClassChar);
        dogClass = static_cast<jclass>(env->NewGlobalRef(temp));//提升全局引用
//        释放class
        env->DeleteLocalRef(temp);
    }
    jmethodID init = env->GetMethodID(dogClass, "<init>", "()V");
    jobject dogObject = env->NewObject(dogClass, init);

    init = env->GetMethodID(dogClass, "<init>", "(I)V");
    dogObject = env->NewObject(dogClass, init, 100);

    init = env->GetMethodID(dogClass, "<init>", "(ILjava/lang/String;)V");
    jstring name = env->NewStringUTF("dsfsdfsd");
    dogObject = env->NewObject(dogClass, init, 100, name);

    env->DeleteLocalRef(dogObject);

}//JNI函数结束，会释放局部引用，dogClass虽然会被释放，但是还不等于NULL，只是一个悬空指针。所以再次调用函数，进不去if语句中

// 可以在这个类中声明，另外一个类去实现
extern int age;//声明age
extern void show();//声明函数


extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_MainActivity_delQuote(JNIEnv *env, jobject thiz) {
    if (dogClass != NULL) {
        env->DeleteGlobalRef(dogClass);
        dogClass = NULL;//指向NULL 避免成为悬空指针
        LOGE("全局引用释放完毕，上面的按钮已经失去了全局引用，再次点击会报错！");
    }
    show();
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ndk_KotlinActivity_stringFromJNIKotlin(JNIEnv *env, jobject thiz) {
}

int compare(const jint *number1, const jint *number2) {
    return *number1 - *number2;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_KotlinActivity_sort(JNIEnv *env, jobject thiz, jintArray arr) {
    jint *intArray = env->GetIntArrayElements(arr, nullptr);
    int length = env->GetArrayLength(arr);
    //NDK 很大的工具链(Java JNI . C++ stdLib)
    qsort(intArray, length, sizeof(int),
          reinterpret_cast<int (*)(const void *, const void *)>(compare));

    env->ReleaseIntArrayElements(arr, intArray, 0);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_StaticCacheActivity_localCache(JNIEnv *env, jclass clazz, jstring name) {
    //像 opencv webrtc 大量使用静态缓存

    //非静态缓存
    jfieldID jfieldId = nullptr;
    if (jfieldId == nullptr) {
        jfieldId = env->GetStaticFieldID(clazz, "name", "Ljava/lang/String;");
    } else {
        LOGE("空的");
    }
    env->SetStaticObjectField(clazz, jfieldId, name);
    jfieldId = nullptr;
}

static jfieldID f_name1 = nullptr;
static jfieldID f_name2 = nullptr;
static jfieldID f_name3 = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_StaticCacheActivity_initStaticCache(JNIEnv *env, jclass clazz) {
    //像 opencv webrtc 大量使用静态缓存
    f_name1 = env->GetStaticFieldID(clazz, "name1", "Ljava/lang/String;");
    f_name2 = env->GetStaticFieldID(clazz, "name2", "Ljava/lang/String;");
    f_name3 = env->GetStaticFieldID(clazz, "name3", "Ljava/lang/String;");

}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_StaticCacheActivity_useStaticCache(JNIEnv *env, jclass clazz, jstring name) {
    //不会反复调用GetStaticFieldID
    env->SetStaticObjectField(clazz, f_name1, name);
    env->SetStaticObjectField(clazz, f_name2, name);
    env->SetStaticObjectField(clazz, f_name3, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_StaticCacheActivity_clearStaticCache(JNIEnv *env, jclass clazz) {
    f_name1 = nullptr;
    f_name2 = nullptr;
    f_name3 = nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_ExceptionActivity_exeption(JNIEnv *env, jclass clazz) {
    //假设现在想操作name999 没有name999就会在native层崩溃
    jfieldID f_id = env->GetStaticFieldID(clazz, "name999", "Ljava/lang/String;");
    //崩溃后，两种解决方案 Caused by: java.lang.NoSuchFieldError: no type "Ljava/lang/String" found and
    // so no field "name999" could be found in class "Lcom/example/ndk/ExceptionActivity;" or its superclasses

    //方案一  补救方案
    jthrowable thr = env->ExceptionOccurred();//检测本次执行，有没有异常
    if (thr) {
        LOGE("C++ 层有异常检测");
        env->ExceptionClear();//此异常清除
        jfieldID f_id = env->GetStaticFieldID(clazz, "name1", "Ljava/lang/String;");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_ExceptionActivity_exeption2(JNIEnv *env, jclass clazz) {
    //方案二  native捕获异常
    jfieldID f_id = env->GetStaticFieldID(clazz, "name999", "Ljava/lang/String;");
    jthrowable thr = env->ExceptionOccurred();//检测本次执行，有没有异常
    if (thr) {
        LOGE("C++ 层有异常检测");
        env->ExceptionClear();
        //Throw抛出一个Java的对象 java/lang/String;    java/xxx/xx/NullExxx;
        jclass jclass1 = env->FindClass("java/lang/NoSuchFieldException");
        env->ThrowNew(jclass1, "NoSuchFieldException 是找不到name999");

    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_ExceptionActivity_exeption3(JNIEnv *env, jclass clazz) {

    jmethodID jmethodId = env->GetStaticMethodID(clazz, "show", "()V");
    env->CallStaticVoidMethod(clazz, jmethodId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();//输出描述信息
        env->ExceptionClear();
    }


//    env->NewStringUTF("sdasda");//局部引用 奔溃处理会被忽略
    //这种方式 前边有调用上方方法 失效


}