#include <jni.h>
#include <string>
#include <android/log.h>
#include <pthread.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"StudyNDK",__VA_ARGS__);

extern "C" JNIEXPORT jstring

JNICALL
Java_com_guidongyuan_studyndk_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    LOGI("Java调用native")
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_guidongyuan_studyndk_MainActivity_passValueToJNI(JNIEnv *env, jobject instance,
                                                          jint intValue, jstring strValue_) {

    LOGI("基本数据类型：intValue = %d", intValue);

    const char *strValue = env->GetStringUTFChars(strValue_, 0);

    LOGI("string数据类型：strValue = %s", strValue);

    // 释放
    env->ReleaseStringUTFChars(strValue_, strValue);

    return env->NewStringUTF("passValueToJNI 回调");
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_guidongyuan_studyndk_MainActivity_passArrayToJNI(JNIEnv *env, jobject instance,
                                                          jintArray intArray_,
                                                          jobjectArray strArray) {
    // 获得基本数据类型数组
    // 参数intArray_类型为jintArray，可以看到是typedef _jintArray* jintArray;这样定义的
    // 传递的为指针地址，指向数组首元素地址

    // 获取数组长度
    int intLength = env->GetArrayLength(intArray_);
    // 如果为Boolean则调用GetBooleanArrayElements不同的对应
    jint *intArray = env->GetIntArrayElements(intArray_, NULL);
    for (int i = 0; i < intLength; ++i) {
        LOGI("int数据为：i = %d ",*(intArray+i));
        // 因为传递的为指针地址，所以，在这里进行修改，会影响到java代码中的值
        *(intArray + i) = *(intArray + i) + 10;
    }
    // 释放
    env->ReleaseIntArrayElements(intArray_, intArray, 0);

    // 获得字符串类型数组
    int strLength = env->GetArrayLength(strArray);
    for (int i = 0; i < strLength; ++i) {
        // object类型转成jstring类型
        jstring str = static_cast<jstring>(env->GetObjectArrayElement(strArray, i));
        // 需要转成char* 类型再输出，否则会出错
        const char *c_str = const_cast<char *>(env->GetStringUTFChars(str, 0));
        LOGI("string数据为 i = %s ",c_str);
        env->ReleaseStringUTFChars(str, c_str);
    }

    return env->NewStringUTF("passArrayToJNI 回调");
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_guidongyuan_studyndk_MainActivity_passBeanToJNI(JNIEnv *env, jobject instance,
                                                         jobject bean) {
    // 传递对象
    jclass beanClass = env->GetObjectClass(bean);

    return env->NewStringUTF("passBeanToJNI 回调");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_guidongyuan_studyndk_MainActivity_callJNI(JNIEnv *env, jobject instance) {
    // 获取jclass对象
    jclass _jclass = env->GetObjectClass(instance);
    // 调用无参方法
    jmethodID methodId = env->GetMethodID(_jclass, "callFromJNI", "()V");
    env->CallVoidMethod(instance, methodId);
    // 调用int类型参数的方法
    jmethodID methodWithIntId = env->GetMethodID(_jclass, "callFromJNI", "(I)V");
    env->CallVoidMethod(instance, methodWithIntId, 100);
    // 调用string类型参数的方法
    jmethodID methodWithStringId = env->GetMethodID(_jclass, "callFromJNI", "(Ljava/lang/String;)V");
    env->CallVoidMethod(instance, methodWithStringId, env->NewStringUTF("callJNI 回调"));

    // 调用静态方法
    jmethodID methodStaticId = env->GetStaticMethodID(_jclass, "callStaticFromJNI", "()V");
    env->CallStaticVoidMethod(_jclass, methodStaticId);

    // 调用int数据类型变量
    jfieldID fieldIntId = env->GetFieldID(_jclass, "age", "I");
    int ageInt = env->GetIntField(instance, fieldIntId);
    LOGI("获取Java变量 age = %d ", ageInt);
    // 修改变量内容
    env->SetIntField(instance, fieldIntId, 19);

    // 调用String数据类型变量
    jfieldID fieldStrId = env->GetFieldID(_jclass, "name", "Ljava/lang/String;");
    jstring nameStr = static_cast<jstring>(env->GetObjectField(instance, fieldStrId));
    const char* nameStrChar = env->GetStringUTFChars(nameStr, 0);
    LOGI("获取Java变量 name = %s ",nameStrChar);
    // 修改string变量内容
    env->SetObjectField(instance, fieldStrId, env->NewStringUTF("Change yuan"));

    // 调用静态变量
    jfieldID fieldStaticId = env->GetStaticFieldID(_jclass, "school", "Ljava/lang/String;");
    jstring schoolStr = static_cast<jstring>(env->GetStaticObjectField(_jclass, fieldStaticId));
    const char* schoolStrChar = env->GetStringUTFChars(schoolStr, 0);
    LOGI("获取Java静态变量 school = %s ",schoolStrChar);
    // 修改static变量内容
    env->SetStaticObjectField(_jclass, fieldStaticId, env->NewStringUTF("Change GDUT"));

    // 创建对象，需要传递完整包路径
    jclass beanClass = env->FindClass("com/guidongyuan/studyndk/Bean");
    jmethodID constructMethodId = env->GetMethodID(beanClass, "<init>", "()V");
    jobject beanObject = env->NewObject(beanClass, constructMethodId);

    jmethodID getIId = env->GetMethodID(beanClass,"getI","()I");
    jint iValue = env->CallIntMethod(beanObject,getIId);
    LOGI("获取Java 变量 i = %d",iValue);

    jmethodID setIId = env->GetMethodID(beanClass, "setI", "(I)V");
    env->CallVoidMethod(beanObject, setIId, 1);
}

JavaVM *_vm;
jobject _instance = 0;


void *pthreaTask(void *args){
    LOGI("pthreaTask");
    // 设置为全局变量
    JNIEnv *env = nullptr;
    jint i = _vm->AttachCurrentThread(&env, 0);
    if (i != JNI_OK){
        return 0;
    }
    jclass _jclass = env->GetObjectClass(_instance);
    jmethodID fieldId = env->GetMethodID(_jclass, "updateUI", "()V");
    env->CallVoidMethod(_instance, fieldId);
    _vm->DetachCurrentThread();
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_guidongyuan_studyndk_MainActivity_createThread(JNIEnv *env, jobject instance) {
    // 把instant设置为全局引用
    _instance = env->NewGlobalRef(instance);
    pthread_t _pthread;
    pthread_create(&_pthread, 0, pthreaTask, instance);
    // 避免线程方法还没有执行，该方法就先执行完毕，把instance释放掉了
    pthread_join(_pthread, 0);
    env->DeleteGlobalRef(_instance);
    LOGI("createThread 执行结束");
}

// 需要动态注册native方法的类名
// 混淆的时候，要注意，不能被混淆，否则会失败
static const char* mClassName = "com/guidongyuan/studyndk/MainActivity";

void dynamicRegisterNative(JNIEnv *env, jobject instance){
    LOGI("dynamicRegisterNative 动态注册");
}

jstring dynamicRegisterNativeStatic(JNIEnv *env, jobject instance, jint i){
    return env->NewStringUTF("dynamicRegisterNativeStatic 动态注册");
}

//需要动态注册的方法数组
//{"java本地方法名","签名",java方法对应jni中的方法名}
static const JNINativeMethod method[] = {
        {"dynamicRegister", "()V", (void *)dynamicRegisterNative},
        {"dynamicRegisterStatic", "(I)Ljava/lang/String;", (jstring *)dynamicRegisterNativeStatic}
};

// Java中执行完System.loadLibrary，就会自动调用该方法
int JNI_OnLoad(JavaVM *vm, void *re) {
    LOGI("调用JNI_Onload");
    _vm = vm;

    JNIEnv *env = nullptr;
    int i = vm->AttachCurrentThread(&env, 0);
    if (i != JNI_OK) {
        return 0;
    }
    jclass mainActivityCls = env->FindClass(mClassName);
    // 传递方法数组的名称和数目
    // 注册 如果小于0则注册失败
    i = env->RegisterNatives(mainActivityCls,method,sizeof(method)/ sizeof(JNINativeMethod));
    if(i != JNI_OK )
    {
        return -1;
    }

    return JNI_VERSION_1_6;
}
