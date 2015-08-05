// JNI����ʵ��
#include "jni_jdk1_6_24\jni.h"
#include "org_jwebtop_JWebTopNative.h"
#include "jwebtop_brige.h"
#include "javautils.h"
#include  "JWebTop/winctrl/JWebTopWinCtrl.h"

typedef jboolean(JNICALL *GETAWT)(JNIEnv*, JAWT*);
typedef BOOL(WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

using namespace std;

JavaVM* g_jvm;// ����ȫ�ֵ����������
jclass g_nativeClass;// ��¼ȫ�ֵı��������
jmethodID g_invokeByJS;// ��C�˻ص�Java�ķ���

extern HINSTANCE g_instance;
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved){
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//HINSTANCE exe_hInstance = ::GetModuleHandle(NULL);
		g_instance = hModule;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		// �ǲ�������Ҫ��Щ�������������ر����������
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
// ���ڻص�java����ķ�����������jwebtop_brige.h��
CefString jw::invokeJavaMethod(CefString json){
	JNIEnv *env;
	if (g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6) < 0){
		if (g_jvm->AttachCurrentThread((void **)&env, NULL) < 0) return CefString();
	}

	jstring sss = env->NewStringUTF(json.ToString().c_str());
	jstring str = (jstring)env->CallStaticObjectMethod(g_nativeClass, g_invokeByJS, sss);
	env->DeleteLocalRef(sss);
	// char * tmp = jstringToWindows(env, str);	
	CefString result = CefString(env->GetStringUTFChars(str, false));
	env->DeleteLocalRef(str);
	// delete tmp;
	return result;
}

// jni���������������
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nCreateJWebTop
(JNIEnv * env, jclass nativeClass, jstring appfile, jlong parentWin
// һ�²������滻appfile�е���Ӱ����
, jstring url       // Ҫ�򿪵����ӵ�ַ
, jstring title     // ��������
, jstring icon      // ����ͼ��
, jint x, jint y    // �������Ͻ�����,��ֵΪ-1ʱ�����ô˱���		 
, jint w, jint h    // ���ڵĿ��ߣ���ֵΪ-1ʱ�����ô˱���		
){
	CefString result = CefString(env->GetStringUTFChars(appfile, false));
	if (g_invokeByJS == NULL){// ��һ�α�java�˵���
		env->GetJavaVM(&g_jvm);// ��ȡ��ǰ���������������������
		g_nativeClass = (jclass)(env->NewGlobalRef(nativeClass));// ��һ����������Ϊȫ�ֶ��󣬴˴���nativeClasss����Ϊȫ�ֶ���
		g_invokeByJS = env->GetStaticMethodID(g_nativeClass, "invokeByJS", "(Ljava/lang/String;)Ljava/lang/String;");// ȡ��Ҫ���õķ���
		// ���������(������������Ҫ�ص�java���Ա㴫�ݴ����������������java��
		LPTSTR url_ = NULL;
		if (url != NULL)url_ = chr2wch(jstringToWindows(env, url));
		LPTSTR title_ = NULL;
		if (title != NULL)title_ = chr2wch(jstringToWindows(env, title));
		LPTSTR icon_ = NULL;
		if (icon != NULL)icon_ = chr2wch(jstringToWindows(env, icon));
		startJWebTop(g_instance/*������dll attach��ʱ���ȡ��*/, LPTSTR(result.ToWString().c_str()), parentWin, url_, title_, icon_, x, y, w, h);
	}
	else{	 //�����ٴδ�����������ڲ��У��ѵ����ڲ�ͬ�̵߳�ԭ�򣿣���jsִ��RunApp��������
		jw::runApp(LPTSTR(result.ToWString().c_str()), parentWin);
	}
}
// jni������ִ�нű�
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJs
(JNIEnv * env, jclass, jlong browserHWnd, jstring json){
	string js;
	// js.clear();	// ���Կ��ǽ�js��Ϊһ��ȫ�ֱ�����ÿ��ʹ��ǰ/��clearһ�£�����Ч���Ƿ���ߣ�
	js = "invokeByJava(";
	const char * tmp = env->GetStringUTFChars(json, false);
	js += tmp;
	env->ReleaseStringUTFChars(json, tmp);
	js += ")";
	jw::ExecJS((HWND)browserHWnd, js);
}


// jni���������ô��ڴ�С
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetSize
(JNIEnv * env, jclass, jlong browserHWnd, jint w, jint h){
	jw::setSize((HWND)browserHWnd, w, h);
}

// jni���������ô���λ��
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetLocation
(JNIEnv * env, jclass, jlong browserHWnd, jint x, jint y){
	jw::move((HWND)browserHWnd, x, y);
}

// jni���������ô���λ�úʹ�С
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetBound
(JNIEnv *, jclass, jlong browserHWnd, jint x, jint y, int w, int h){
	jw::setBound((HWND)browserHWnd, x, y, w, h);
}

// jni�����������µ���ҳ��ַ
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetUrl
(JNIEnv * env, jclass, jlong browserHWnd, jstring url){
	jw::loadUrl((HWND)browserHWnd, chr2wch(jstringToWindows(env, url)));
}