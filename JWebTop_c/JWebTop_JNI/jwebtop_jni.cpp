// JNI����ʵ��
#include "jni_jdk1_6_24\jni.h"
#include <Shlwapi.h>
#include "common/task/Task.h"
#include "common/JWebTopMsg.h"
#include "common/util/StrUtil.h"
#include "common/msgwin/MsgWin.h"
#include "common/winctrl/JWebTopWinCtrl.h"

#include "org_jwebtop_JWebTopNative.h"
#include "common_dll/jwebtop_dll.h"
#include "javautils.h"
typedef jboolean(JNICALL *GETAWT)(JNIEnv*, JAWT*);
using namespace std;

JavaVM* g_jvm;               // ����ȫ�ֵ����������
jclass g_nativeClass;        // ��¼ȫ�ֵı��������
jmethodID g_invokeByJS;      // ��C�˻ص�Java�ķ���
extern HWND g_RemoteWinHWnd;

// ʵ�ֻص�java����ķ���
wstring invokeByBrowser(long browserHWnd, wstring json){
	JNIEnv *env;
	bool detachEnv = false;
	if (g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6) < 0){
		if (g_jvm->AttachCurrentThread((void **)&env, NULL) < 0){
			return wstring();
		}
		else{
			detachEnv = true;// �����Attach�ϵ�env����ô��Ҫdetach
		}
	}
	jstring sss = env->NewStringUTF(CefString(json).ToString().c_str());
	jstring str = (jstring)env->CallStaticObjectMethod(g_nativeClass, g_invokeByJS, (jlong)browserHWnd, sss);
	env->DeleteLocalRef(sss);
	CefString result(env->GetStringUTFChars(str, false));
	env->DeleteLocalRef(str);
	if (detachEnv)g_jvm->DetachCurrentThread();
	return result.ToWString();
}

// jni����������JWebTop����
// ������ʱû�а취��JSON�ķ�ʽ���������ã���Ϊ��cmd��ʽ��������ʱ��ϵͳ��Ĭ�ϰѲ����е�˫���Ÿ�ȥ�������ߣ��ȴ������ڣ��ٴ������������
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nCreateJWebTop
(JNIEnv * env, jclass nativeClass, jstring jprocessPath, jstring configFile){
	if (g_RemoteWinHWnd != NULL)return /*(jlong)g_RemoteWinHWnd*/;
	if (g_invokeByJS == NULL){// ��һ�α�java�˵���
		env->GetJavaVM(&g_jvm);// ��ȡ��ǰ���������������������		
		g_nativeClass = (jclass)(env->NewGlobalRef(nativeClass));// ��һ����������Ϊȫ�ֶ��󣬴˴���nativeClasss����Ϊȫ�ֶ���
		g_invokeByJS = env->GetStaticMethodID(g_nativeClass, "invokeByJS", "(JLjava/lang/String;)Ljava/lang/String;");// ȡ��Ҫ���õķ���
	}
	wstring processPath = jstring2wstring(env, jprocessPath); 
	wstring cfgFile = jstring2wstring(env, configFile);
	CreateJWebTop(processPath, cfgFile);
}
// jni�������˳�JWebTop����
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExit(JNIEnv *, jclass){
	ExitJWebTop();
}

/*
* ��Ӧorg.jwebtop.JWebTopNative���nCreateBrowser����
* �÷������ڴ���һ�����������
* jWebTopConfigJSON �����������ϢJSON
*/
JNIEXPORT jlong JNICALL Java_org_jwebtop_JWebTopNative_nCreateBrowser
(JNIEnv * env, jclass, jstring jWebTopConfigJSON){
	wstring cmds = jstring2wstring(env, jWebTopConfigJSON);
	return CreateJWebTopBrowser(cmds);
}

/*
 * ��Ӧorg.jwebtop.JWebTopNative���nCloseBrowser����
 * �÷������ڹر�һ�����������
 * browserHWnd  ��������ھ��
 */
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nCloseBrowser
(JNIEnv * env, jclass, jlong browserHWnd){
	CloseJWebTopBrowser(browserHWnd);
}

JNIEXPORT jstring JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring script){
	return wstring2jstring(env, JWebTopExecuteJSWait(browserHWnd, jstring2wstring(env, script)));
}

JNIEXPORT jstring JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSONWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring json){
	return wstring2jstring(env, JWebTopExecuteJSONWait(browserHWnd, jstring2wstring(env, json)));
}

JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSNoWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring script){
	JWebTopExecuteJSNoWait(browserHWnd, jstring2wstring(env, script));
}

// jni������ִ�нű��Ҳ��ȴ����ؽ��
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSONNoWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring json){
	JWebTopExecuteJSONNoWait(browserHWnd, jstring2wstring(env, json));
}

// jni���������ô��ڴ�С
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetSize
(JNIEnv * env, jclass, jlong browserHWnd, jint w, jint h){
	jw::setSize((HWND)browserHWnd, w, h);
}
JNIEXPORT jintArray JNICALL Java_org_jwebtop_JWebTopNative_nGetSize
(JNIEnv * env, jclass, jlong browserHWnd){
	POINT p = jw::getSize((HWND)browserHWnd);
	jintArray rtn = env->NewIntArray(2);
	jint * tmp = new jint[2];
	tmp[0] = p.x;
	tmp[1] = p.y;
	delete[] tmp;
	env->SetIntArrayRegion(rtn, 0, 2, tmp);
	return rtn;
}
JNIEXPORT jintArray JNICALL Java_org_jwebtop_JWebTopNative_nGetScreenSize
(JNIEnv * env, jclass){
	POINT p = jw::getScreenSize();
	jintArray rtn = env->NewIntArray(2);
	jint * tmp = new jint[2];
	tmp[0] = p.x;
	tmp[1] = p.y;
	delete[] tmp;
	env->SetIntArrayRegion(rtn, 0, 2, tmp);
	return rtn;
}
// jni���������ô���λ��
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetLocation
(JNIEnv * env, jclass, jlong browserHWnd, jint x, jint y){
	jw::move((HWND)browserHWnd, x, y);
}
JNIEXPORT jintArray JNICALL Java_org_jwebtop_JWebTopNative_nGetLocation
(JNIEnv * env, jclass, jlong browserHWnd){
	POINT p = jw::getPos((HWND)browserHWnd);
	jintArray rtn = env->NewIntArray(2);
	jint * tmp = new jint[2];
	tmp[0] = p.x;
	tmp[1] = p.y;
	delete[] tmp;
	env->SetIntArrayRegion(rtn, 0, 2, tmp);
	return rtn;
}

// jni���������ô���λ�úʹ�С
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetBound
(JNIEnv *, jclass, jlong browserHWnd, jint x, jint y, int w, int h){
	jw::setBound((HWND)browserHWnd, x, y, w, h);
}
JNIEXPORT jintArray JNICALL Java_org_jwebtop_JWebTopNative_nGetBound
(JNIEnv * env, jclass, jlong browserHWnd){
	RECT rc = jw::getBound((HWND)browserHWnd);
	jintArray rtn = env->NewIntArray(4);
	jint * tmp = new jint[4];
	tmp[0] = rc.left;
	tmp[1] = rc.top;
	tmp[2] = rc.right - rc.left;
	tmp[3] = rc.bottom - rc.top;
	delete[] tmp;
	env->SetIntArrayRegion(rtn, 0, 4, tmp);
	return rtn;
}

//�����Ƶ����
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nBringToTop
(JNIEnv *, jclass, jlong browserHWnd){
	jw::bringToTop((HWND)browserHWnd);
}
//ʹ���ڻ�ý���
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nFocus
(JNIEnv *, jclass, jlong browserHWnd){
	jw::focus((HWND)browserHWnd);
}
//���ش���
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nHide
(JNIEnv *, jclass, jlong browserHWnd){
	jw::hide((HWND)browserHWnd);
}
//��󻯴���
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nMax
(JNIEnv *, jclass, jlong browserHWnd){
	jw::max((HWND)browserHWnd);
}
//��С������
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nMini
(JNIEnv *, jclass, jlong browserHWnd){
	jw::mini((HWND)browserHWnd);
}
//��ԭ���ڣ���Ӧ��hide����
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nRestore
(JNIEnv *, jclass, jlong browserHWnd){
	jw::restore((HWND)browserHWnd);
}

// �����Ƿ���ʾ
JNIEXPORT jboolean JNICALL Java_org_jwebtop_JWebTopNative_nIsVisible(JNIEnv *, jclass, jlong browserHWnd){
	return jw::isVisible((HWND)browserHWnd);
}
//�����ö����˺�����bringToTop���������ڴ˺�����ʹ������Զ�ö�������������һ�����ڵ������ö�����
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetTopMost
(JNIEnv *, jclass, jlong browserHWnd){
	jw::setTopMost((HWND)browserHWnd);
}

JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetWindowStyle
(JNIEnv *, jclass, jlong browserHWnd,jint style){
	jw::setWindowStyle((HWND)browserHWnd,style);
}
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetWindowExStyle
(JNIEnv *, jclass, jlong browserHWnd, jint exStyle){
	jw::setWindowExStyle((HWND)browserHWnd, exStyle);
}

JNIEXPORT jintArray Java_org_jwebtop_JWebTopNative_nGetWindowClient(JNIEnv * env, jclass, jlong hWnd){
	WINDOWINFO winInfo;
	GetWindowInfo((HWND)hWnd, &winInfo);// ��ȡ������Ϣ
	RECT rc = winInfo.rcClient;
	jintArray rtn = env->NewIntArray(4);
	jint * tmp = new jint[4];
	tmp[0] = rc.left;
	tmp[1] = rc.top;
	tmp[2] = rc.right;
	tmp[3] = rc.bottom;
	delete[] tmp;
	env->SetIntArrayRegion(rtn, 0, 4, tmp);
	return rtn;
}