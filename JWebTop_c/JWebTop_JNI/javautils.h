#include <windows.h> 
#include "jni_jdk1_6_24/jni.h"
#include "common/util/StrUtil.h"

JNIEnv* GetJNIEnv();
void DetachFromThread(bool *mustDetach);

//��java���ַ���ת����windows���ַ���   
char*   jstringToWindows(JNIEnv*   env, jstring   jstr);