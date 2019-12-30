#include <jni.h>
#include <string>
#include "JniUtils.h"

using namespace std;

/**
 * 获取Context对象
 * @param env
 * @return
 */
static jobject getApplication(JNIEnv *env) {
    jobject application = NULL;
    //反射ActivityThread
    jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
    if (activity_thread_clz != NULL) {
        //反射currentApplication方法
        jmethodID currentApplication = env->GetStaticMethodID(
                activity_thread_clz, "currentApplication", "()Landroid/app/Application;");
        if (currentApplication != NULL) {
            //执行反射currentApplication方法
            application = env->CallStaticObjectMethod(activity_thread_clz, currentApplication);
        } else {
            LOGE("Cannot find method: currentApplication() in ActivityThread.");
        }
        //释放内存
        env->DeleteLocalRef(activity_thread_clz);
    } else {
        LOGE("Cannot find class: android.app.ActivityThread");
    }
    return application;
}

/**
 * 反射getSharedPreferences获取SharedPreferences对象
 * @param env
 * @param fileName
 * @return
 */
jobject getSharedPreferencesNative(JNIEnv *env,const char *fileName) {
    //反射Context类
    jclass cls_Context = env->FindClass("android/content/Context");
    //反射Context类getSharedPreferences方法
    jmethodID mid_getSharedPreferences = env->GetMethodID(cls_Context,
                                                          "getSharedPreferences",
                                                          "(Ljava/lang/String;I)Landroid/content/SharedPreferences;");
    //反射Context类MODE_PRIVATE属性
    jfieldID fid_MODE_PRIVATE = env->GetStaticFieldID(cls_Context, "MODE_PRIVATE", "I");
    //获取Context类MODE_PRIVATE属性值
    jint int_MODE_PRIVATE = env->GetStaticIntField(cls_Context, fid_MODE_PRIVATE);
    //参数类型转换
    jstring o_prefs_name = Str2Jstring(env, fileName);
    //执行反射方法
    jobject obj_sharedPreferences = env->CallObjectMethod(getApplication(env),
                                                          mid_getSharedPreferences, o_prefs_name,
                                                          int_MODE_PRIVATE);
    //释放内存
    env->DeleteLocalRef(cls_Context);
    env->DeleteLocalRef(o_prefs_name);
    return obj_sharedPreferences;
}

/**
 * 设置key值
 * @param env
 * @param fileName
 * @param keyName
 * @param value
 * @return
 */
bool setKey(JNIEnv *env,const char *fileName,const char *keyName, const char *value) {
    //反射getSharedPreferences获取SharedPreferences对象
    jobject obj_sharedPreferences = getSharedPreferencesNative(env, fileName);
    //反射SharedPreferences
    jclass cls_SharedPreferences = env->FindClass("android/content/SharedPreferences");
    //反射SharedPreferences$Editor类
    jclass cls_SharedPreferencesEditor = env->FindClass("android/content/SharedPreferences$Editor");
    //反射edit方法
    jmethodID mid_edit = env->GetMethodID(cls_SharedPreferences,
                                          "edit", "()Landroid/content/SharedPreferences$Editor;");
    //反射putString方法
    jmethodID mid_putString = env->GetMethodID(cls_SharedPreferencesEditor,
                                               "putString",
                                               "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;");
    //反射commit方法
    jmethodID mid_commit = env->GetMethodID(cls_SharedPreferencesEditor, "commit", "()Z");
    //执行edit方法
    jobject obj_editor = env->CallObjectMethod(obj_sharedPreferences, mid_edit);
    //参数类型转换
    jstring str_keyLicense = Str2Jstring(env, keyName);
    //参数类型转换
    jstring str_value = Str2Jstring(env, value);
    //执行putString方法
    env->CallObjectMethod(obj_editor, mid_putString, str_keyLicense, str_value);
    //执行commit方法
    jboolean jb_commit = env->CallBooleanMethod(obj_editor, mid_commit);
    //释放内存
    env->DeleteLocalRef(cls_SharedPreferences);
    env->DeleteLocalRef(cls_SharedPreferencesEditor);
    env->DeleteLocalRef(obj_sharedPreferences);
    env->DeleteLocalRef(obj_editor);
    env->DeleteLocalRef(str_keyLicense);
    env->DeleteLocalRef(str_value);
    return jb_commit;
}
/**
 * 读取key值
 * @param env
 * @param fileName
 * @param keyName
 * @param defValue
 * @return
 */
jstring getKey(JNIEnv *env,const char *fileName,const char *keyName, const char *defValue) {
    //反射getSharedPreferences获取SharedPreferences对象
    jobject obj_sharedPreferences = getSharedPreferencesNative(env, fileName);
    //反射SharedPreferences类
    jclass cls_SharedPreferences = env->FindClass("android/content/SharedPreferences");
    //反射SharedPreferences类的getString方法
    jmethodID mid_getString = env->GetMethodID(cls_SharedPreferences,
                                               "getString",
                                               "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
    //参数类型转换
    jstring key_name = env->NewStringUTF(keyName);
    //参数类型转换
    jstring default_value = env->NewStringUTF(defValue);
    //执行反射方法
    jstring str_keyLicenseValue = (jstring) env->CallObjectMethod(obj_sharedPreferences,
                                                                  mid_getString, key_name, default_value);
    //释放内存
    env->DeleteLocalRef(obj_sharedPreferences);
    env->DeleteLocalRef(cls_SharedPreferences);
    env->DeleteLocalRef(key_name);
    env->DeleteLocalRef(default_value);
    return str_keyLicenseValue;
}

/**
 * 反射getSharedPreferences获取SharedPreferences对象
 * @param env
 * @param fileName
 * @return
 */
jstring getPackageNameNative(JNIEnv *env) {
    //反射Context类
    jclass cls_Context = env->FindClass("android/content/Context");
    //反射Context类getPackageName方法
    jmethodID mid_getPackageName = env->GetMethodID(cls_Context,
                                                          "getPackageName",
                                                          "()Ljava/lang/String;");
    //执行反射方法
    jstring packageName =(jstring) env->CallObjectMethod(getApplication(env),
                                                          mid_getPackageName);
    //释放内存
    env->DeleteLocalRef(cls_Context);
    return packageName;
}

extern "C" JNIEXPORT jstring JNICALL
Java_cn_ahwind_jnisharepreference_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    return getKey(env,jstringToChar(env,getPackageNameNative(env)),"native_key","hello from jni");
}

