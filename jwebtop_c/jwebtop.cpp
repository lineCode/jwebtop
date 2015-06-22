// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
// #include "stdafx.h"
#include "jni_jdk1_6_24\jni.h"
#include <iostream>
#include "org_jwebtop_JWebTopNative.h"
#include "jwebtop_brige.h"
#include "javautils.h"
#include "transparent_wnd.h"

typedef jboolean(JNICALL *GETAWT)(JNIEnv*, JAWT*);
typedef BOOL(WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

using namespace std;

extern TransparentWnd* pTransparentBrowser1;
HINSTANCE g_hIntance;

JavaVM* g_jvm;// ����ȫ�ֵ����������
jclass g_nativeClass;// ��¼ȫ�ֵı����������com.toone.util.natives.NativeMethods
jmethodID g_invokeByJS;// �����豸ʱ�Ļص�����

BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved){
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
// ���ڻص�java����ķ�����������jwebtop_brige.h��
CefString invokeJavaMethod(CefString json){
	JNIEnv *env;
	if (g_jvm->AttachCurrentThread((void **)&env, NULL) < 0) return CefString();
	jstring sss = env->NewStringUTF(json.ToString().c_str());
	jstring str = (jstring)env->CallStaticObjectMethod(g_nativeClass, g_invokeByJS,sss);
	env->DeleteLocalRef(sss);
	char * tmp = jstringToWindows(env, str);	
	// MessageBox(NULL, chr2wch(tmp), L"ͨ��Javaת����Ľ��", 1);
	return CefString(env->GetStringUTFChars(str, false));
}

// jni���������������
JNIEXPORT jlong JNICALL Java_org_jwebtop_JWebTopNative_createJWebTop
(JNIEnv * env, jclass nativeClass, jstring appfile, jlong parentWin){
	const char * tmp = env->GetStringUTFChars(appfile, false);
	wchar_t* buffer = chr2wch(tmp);
	env->ReleaseStringUTFChars(appfile, tmp);
	if (g_invokeByJS == NULL){// ��һ�α�java�˵���
		env->GetJavaVM(&g_jvm);// ��ȡ��ǰ���������������������
		g_nativeClass = (jclass)(env->NewGlobalRef(nativeClass));// ��һ����������Ϊȫ�ֶ��󣬴˴���nativeClasss����Ϊȫ�ֶ���
		g_invokeByJS = env->GetStaticMethodID(g_nativeClass, "invokeByJS", "(Ljava/lang/String;)Ljava/lang/String;");// ȡ��Ҫ���õķ���
	}
	wWinMain(NULL, 0, buffer, 1);// ���������(������������Ҫ�ص�java���Ա㴫�ݴ����������������java��
	delete buffer;
	return 0;// ���ڴ��������������ʽ�ģ���������ķ��ظ�����������
}

// jni������ִ�нű�
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_executeJs
(JNIEnv * env, jclass, jlong browserHWnd, jstring json){
	string js;
	// js.clear();	// ���Կ��ǽ�js��Ϊһ��ȫ�ֱ�����ÿ��ʹ��ǰ/��clearһ�£�����Ч���Ƿ���ߣ�
	js = "invokeByJava(";
	const char * tmp = env->GetStringUTFChars(json, false);
	js += tmp;
	env->ReleaseStringUTFChars(json, tmp);
	js += ")";
	TransparentWnd* winHandler = (TransparentWnd*)static_cast<long>(browserHWnd);
	winHandler->ExecJS(js);
}
