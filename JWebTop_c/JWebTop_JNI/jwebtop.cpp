// JNI����ʵ��
#include "jni_jdk1_6_24\jni.h"
#include <Shlwapi.h>
#include "common/task/Task.h"
#include "common/JWebTopMsg.h"
#include "common/util/StrUtil.h"
#include "common/winctrl/JWebTopWinCtrl.h"

#include "org_jwebtop_JWebTopNative.h"
#include "javautils.h"
#include "common/msgwin/MsgWin.h"
typedef jboolean(JNICALL *GETAWT)(JNIEnv*, JAWT*);
typedef BOOL(WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
using namespace std;
extern JavaVM* g_jvm;// ����ȫ�ֵ����������
jclass g_nativeClass;// ��¼ȫ�ֵı��������
jmethodID g_invokeByJS;// ��C�˻ص�Java�ķ���
extern HWND g_RemoteWinHWnd;

//extern HINSTANCE g_instance;
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved){
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//HINSTANCE exe_hInstance = ::GetModuleHandle(NULL);
		//g_instance = hModule;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		// �ǲ�������Ҫ��Щ�������������ر����������
		break;
	case DLL_PROCESS_DETACH:
		//jw::sendProcessMsg(g_RemoteWinHWnd, WM_COPYDATA_EXIT, L"");// �����쳣����ʱ�߲����˴�����������û��
		break;
	}
	return TRUE;
}
// ���ڻص�java����ķ�����������jwebtop_brige.h��
wstring invoke_Wait(DWORD msgId, wstring json){
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
	jstring str = (jstring)env->CallStaticObjectMethod(g_nativeClass, g_invokeByJS, sss);
	env->DeleteLocalRef(sss);
	char * tmp = jstringToWindows(env, str);
	CefString result(env->GetStringUTFChars(str, false));
	env->DeleteLocalRef(str);
	free(tmp);
	if (detachEnv)g_jvm->DetachCurrentThread();
	return result.ToWString();
}

void jw::msgwin_thread_executeWmCopyData( DWORD msgId, wstring json){
	if (msgId == JWM_RESULT_RETURN){// Զ����������ɣ�����������ˣ���Ҫ֪ͨ�����̵ĵȴ��߳�ȥ��ȡ���
		wstring taskId, result;
		long browserHWnd;
		jw::parseMessageJSON(json, ref(browserHWnd), ref(taskId), ref(result));  // ��������Ϣ�н���������id����������
		jw::task::putTaskResult(taskId, result);						   // ֪ͨ�ȴ��̣߳�Զ����������ɣ������ȥȡ��
	}
	else if (msgId == JWM_DLL_EXECUTE_WAIT){// Զ�̽��̷���һ�����񣬲���Զ�̽������ڵȴ���������ɺ���Ҫ����JWEBTOP_MSG_RESULT_RETURN��Ϣ��Զ�̽���
		wstring remoteTaskId, taskInfo;
		long browserHWnd;
		jw::parseMessageJSON(json, ref(browserHWnd), ref(remoteTaskId), ref(taskInfo));        // ��������Ϣ�н���������id����������
		wstring result = invoke_Wait(msgId, taskInfo); 		         // ȡ��ִ�н��
		wstring wrapped = jw::wrapAsTaskJSON(browserHWnd,ref(remoteTaskId), ref(result));      // ��װ�������
		sendProcessMsg((HWND)browserHWnd,JWM_RESULT_RETURN, LPTSTR(wrapped.c_str())); // ���ͽ����Զ�̽���
	}
	else{// ���������Զ�̷���������ȴ�����ִ��
		invoke_Wait(msgId, json);
	}
}

// ����createWin���лص�
void jw::onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine){
	LPTSTR exe = L"D:\\c\\jwebtop\\JWebTop_c\\JWebTop\\Release\\JWebTop.exe";
	//LPTSTR exe = L"JWebTop.exe";
	wstringstream cmd;
	cmd << L": ";// ���Ϊ��������
	cmd << (long)hWnd << " ";// ׷��msgWin��HWND
	cmd << szCmdLine;// ���������в���
	createSubProcess(exe, LPTSTR(cmd.str().c_str()));
}
void appendCMd(JNIEnv * env, wstring * cmd, jstring url){
	if (url != NULL){
		LPTSTR s = LPTSTR(jw::s2w(jstringToWindows(env, url)).c_str());
		if (s[0] == L'"'){// ���url������˫���ſ�ͷ����ô����Ͳ������˫����
			cmd->append(L" ");
			cmd->append(s);
		}
		else{
			cmd->append(L" \"");
			cmd->append(s);
			cmd->append(L"\"");
		}
	}
	else{
		cmd->append(L" :");// ð����Ϊ������Ų���������ļ�·����
	}
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
	//if (appfile == NULL)return -1;
	wstringstream cmd;
	cmd << " " << parentWin;
	cmd << " " << x << " " << y << " " << w << " " << h;// xywh��������null��ֻ������-1���������ȴ���
	wstring cmds = cmd.str();
	appendCMd(env, &cmds, appfile);
	appendCMd(env, &cmds, url);
	appendCMd(env, &cmds, icon);
	if (title != NULL){// title�����
		cmds.append(L" ").append(jw::s2w(jstringToWindows(env, url)).c_str());
	}
	if (g_invokeByJS == NULL){// ��һ�α�java�˵���
		env->GetJavaVM(&g_jvm);// ��ȡ��ǰ���������������������
		g_nativeClass = (jclass)(env->NewGlobalRef(nativeClass));// ��һ����������Ϊȫ�ֶ��󣬴˴���nativeClasss����Ϊȫ�ֶ���
		g_invokeByJS = env->GetStaticMethodID(g_nativeClass, "invokeByJS", "(Ljava/lang/String;)Ljava/lang/String;");// ȡ��Ҫ���õķ���
		// ���������(������������Ҫ�ص�java���Ա㴫�ݴ����������������java��
		jw::createWin(::GetModuleHandle(NULL), LPTSTR(cmds.c_str()));// createWin�Ὺ���µ��߳�
		//startJWebTop(g_instance/*������dll attach��ʱ���ȡ��*/, LPTSTR(result.ToWString().c_str()), parentWin, url_, title_, icon_, x, y, w, h);
	}
	else{	 //�����ٴδ�����������ڲ��У��ѵ����ڲ�ͬ�̵߳�ԭ�򣿣���jsִ��RunApp��������
		//jw::runApp(LPTSTR(result.ToWString().c_str()), parentWin);
	}
}
// jni������ִ�нű��Ҳ��ȴ����ؽ��
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJs
(JNIEnv * env, jclass, jlong browserHWnd, jstring json){
	string js;
	// js.clear();	// ���Կ��ǽ�js��Ϊһ��ȫ�ֱ�����ÿ��ʹ��ǰ/��clearһ�£�����Ч���Ƿ���ߣ�
	js = "invokeByJava(";
	const char * tmp = env->GetStringUTFChars(json, false);
	js += tmp;
	env->ReleaseStringUTFChars(json, tmp);
	js += ")";
	//jw::ExecJS((HWND)browserHWnd, js);
	jw::sendProcessMsg((HWND)browserHWnd, JWM_JSON_EXECUTE_RETURN, LPTSTR(jw::s2w(js).c_str()));
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
	jw::sendProcessMsg((HWND)browserHWnd, JWEBTOP_MSG_LOADURL, LPTSTR(jw::s2w(jstringToWindows(env, url)).c_str()));
	//jw::loadUrl((HWND)browserHWnd, chr2wch(jstringToWindows(env, url)));
}

// jni�������˳�JWebTop����
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExit
(JNIEnv *, jclass){
	jw::sendProcessMsg(g_RemoteWinHWnd, WM_COPYDATA_EXIT,L"");
}