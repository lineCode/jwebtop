// JNI����ʵ��
#include "jni_jdk1_6_24\jni.h"
#include <Shlwapi.h>
#include "common/task/Task.h"
#include "common/JWebTopMsg.h"
#include "common/util/StrUtil.h"
#include "common/process/MultiProcess.h"
#include "common/winctrl/JWebTopWinCtrl.h"

#include "org_jwebtop_JWebTopNative.h"
#include "jwebtop_brige.h"
#include "javautils.h"
#include "msgwin/MsgWin.h"
typedef jboolean(JNICALL *GETAWT)(JNIEnv*, JAWT*);
typedef BOOL(WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
using namespace std;
extern JavaVM* g_jvm;// ����ȫ�ֵ����������
jclass g_nativeClass;// ��¼ȫ�ֵı��������
jmethodID g_invokeByJS;// ��C�˻ص�Java�ķ���

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
	/*jchar s;
	jstring sss =env->NewString(json.c_str, json.size());*/
	//char* jsonChars = wch2chr(json.c_str());
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

void thread_executeWmCopyData(HWND hWnd, DWORD msgId, wstring json){
if (msgId == JWEBTOP_MSG_RESULT_RETURN){// Զ����������ɣ�����������ˣ���Ҫ֪ͨ�����̵ĵȴ��߳�ȥ��ȡ���
		wstring taskId, result;
		long remoteHWnd = 0;
		jw::parseMessageJSON(json, remoteHWnd, ref(taskId), ref(result));  // ��������Ϣ�н���������id����������
		jw::task::putTaskResult(taskId, result);						   // ֪ͨ�ȴ��̣߳�Զ����������ɣ������ȥȡ��
	}
	else if (msgId == JWEBTOP_MSG_EXECUTE_WAIT){// Զ�̽��̷���һ�����񣬲���Զ�̽������ڵȴ���������ɺ���Ҫ����JWEBTOP_MSG_RESULT_RETURN��Ϣ��Զ�̽���
		wstring remoteTaskId, taskInfo;
		long remoteHWnd = 0;
		jw::parseMessageJSON(json, remoteHWnd, ref(remoteTaskId), ref(taskInfo));        // ��������Ϣ�н���������id����������
		wstring result = invoke_Wait(msgId, taskInfo); 		         // ȡ��ִ�н��
		wstring wrapped = jw::wrapAsTaskJSON((long)hWnd, std::ref(remoteTaskId), std::ref(result));      // ��װ�������
		sendProcessMsg((HWND)remoteHWnd, JWEBTOP_MSG_RESULT_RETURN, LPTSTR(wrapped.c_str())); // ���ͽ����Զ�̽���
	}
	else{// ���������Զ�̷���������ȴ�����ִ��
		invoke_Wait(msgId, json);
	}
}
// ����WM_COPYDATA��Ϣ:jwebTop�ص�java����
LRESULT onWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	COPYDATASTRUCT* copyData = (COPYDATASTRUCT*)lParam;
	wstring msg;
	DWORD msgId;
	switch (copyData->dwData)
	{
	case WM_COPYDATA_MINI:{
							  MPMSG_MINI * mpMsg = ((MPMSG_MINI *)(copyData->lpData));
							  msgId = mpMsg->msgId;
							  msg = wstring(mpMsg->msg);
							  break;
	}
	case WM_COPYDATA_LARGE:{
							   MPMSG_LARGE * mpMsg = ((MPMSG_LARGE *)(copyData->lpData));
							   msgId = mpMsg->msgId;
							   msg = wstring(mpMsg->msg);
							   break;
	}
	default:
		break;
	}
	std::thread t(thread_executeWmCopyData, hWnd, msgId, msg);// onWmCopyData��ͬ����Ϣ��Ϊ�˷�ֹ��һ���̵ĵȴ������������߳��н���ҵ����
	t.detach();// �ӵ�ǰ�̷߳���
	return JWEBTOP_MSG_SUCCESS;
}
HWND msgWinHWnd;
// ����createWin���лص�
void onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine){
	msgWinHWnd = hWnd;
	LPTSTR exe = L"D:\\c\\jwebtop\\JWebTop_c\\JWebTop\\Release\\JWebTop.exe";
	wstringstream cmd;
	cmd << L": ";// ���Ϊ��������
	cmd << (long)hWnd << " ";// ׷��msgWin��HWND
	cmd << szCmdLine;// ���������в���
	createSubProcess(exe, LPTSTR(cmd.str().c_str()));
}
void appendCMd(JNIEnv * env, wstring * cmd, jstring url){
	if (url != NULL){
		LPTSTR s = chr2wch(jstringToWindows(env, url));
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
		cmds.append(L" ").append(chr2wch(jstringToWindows(env, title)));
	}
	if (g_invokeByJS == NULL){// ��һ�α�java�˵���
		env->GetJavaVM(&g_jvm);// ��ȡ��ǰ���������������������
		g_nativeClass = (jclass)(env->NewGlobalRef(nativeClass));// ��һ����������Ϊȫ�ֶ��󣬴˴���nativeClasss����Ϊȫ�ֶ���
		g_invokeByJS = env->GetStaticMethodID(g_nativeClass, "invokeByJS", "(Ljava/lang/String;)Ljava/lang/String;");// ȡ��Ҫ���õķ���
		// ���������(������������Ҫ�ص�java���Ա㴫�ݴ����������������java��
		createWin(::GetModuleHandle(NULL), LPTSTR(cmds.c_str()));// createWin�Ὺ���µ��߳�
		//startJWebTop(g_instance/*������dll attach��ʱ���ȡ��*/, LPTSTR(result.ToWString().c_str()), parentWin, url_, title_, icon_, x, y, w, h);
	}
	else{	 //�����ٴδ�����������ڲ��У��ѵ����ڲ�ͬ�̵߳�ԭ�򣿣���jsִ��RunApp��������
		//jw::runApp(LPTSTR(result.ToWString().c_str()), parentWin);
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
	//jw::ExecJS((HWND)browserHWnd, js);
	sendProcessMsg((HWND)browserHWnd, JWEBTOP_MSG_EXECJS, chr2wch(js.c_str()));
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
	sendProcessMsg((HWND)browserHWnd, JWEBTOP_MSG_LOADURL, chr2wch(jstringToWindows(env, url)));
	//jw::loadUrl((HWND)browserHWnd, chr2wch(jstringToWindows(env, url)));
}