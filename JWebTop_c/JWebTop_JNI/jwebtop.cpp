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

JavaVM* g_jvm;               // ����ȫ�ֵ����������
jclass g_nativeClass;        // ��¼ȫ�ֵı��������
jmethodID g_invokeByJS;      // ��C�˻ص�Java�ķ���
extern HWND g_RemoteWinHWnd;
wstring processPath;
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
	CefString result(env->GetStringUTFChars(str, false));
	env->DeleteLocalRef(str);
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
	wstringstream cmd;
	cmd << L": ";// ���Ϊ��������
	cmd << (long)hWnd << " ";// ׷��msgWin��HWND
	cmd << szCmdLine;// ���������в���
	createSubProcess(LPTSTR(processPath.c_str()), LPTSTR(cmd.str().c_str()));
}
void appendCMd(JNIEnv * env, wstring * cmd, const jstring &url){
	if (url != NULL){
		wstring s = jstring2wstring(env, url);
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
void thread_CreateJWebTop(std::wstring cmds)
{
	jw::createWin(::GetModuleHandle(NULL), LPTSTR(cmds.c_str()));// createWin�Ὺ���µ��߳�
}
// jni����������JWebTop����
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nCreateJWebTop
(JNIEnv * env, jclass nativeClass, jstring jprocessPath, jstring configFile){
	if (g_RemoteWinHWnd != NULL)return /*(jlong)g_RemoteWinHWnd*/;
	if (g_invokeByJS == NULL){// ��һ�α�java�˵���
		env->GetJavaVM(&g_jvm);// ��ȡ��ǰ���������������������		
		g_nativeClass = (jclass)(env->NewGlobalRef(nativeClass));// ��һ����������Ϊȫ�ֶ��󣬴˴���nativeClasss����Ϊȫ�ֶ���
		g_invokeByJS = env->GetStaticMethodID(g_nativeClass, "invokeByJS", "(Ljava/lang/String;)Ljava/lang/String;");// ȡ��Ҫ���õķ���
	}
	processPath = jstring2wstring(env, jprocessPath);
	wstring taskId = jw::task::createTaskId();			         // ��������id	
	wstring cmds(taskId);                                        // ������id�ŵ����������� 
	wstring cfgFile = jstring2wstring(env, configFile);
	if (cfgFile[0] != L'\"')cfgFile = L"\"" + cfgFile + L"\"";   // ��˫���Ž�·����������
	cmds.append(L" ").append(cfgFile);	                         // �������������configFile
	jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // �������������
	thread t(thread_CreateJWebTop, cmds); t.detach();			 // �����߳��������������
	lock->wait();		             		 		             // �ȴ��������
	wstring result = lock->result;   		 		             // ȡ��ִ�н��
	long tmp = jw::parseLong(result);
	if (tmp != 0) g_RemoteWinHWnd = (HWND)tmp;
	//return tmp;
}

/*
* ��Ӧorg.jwebtop.JWebTopNative���nCreateBrowser����
* �÷������ڴ���һ�����������
* appFile    ��������ݴ������ļ����г�ʼ��
* parentWin  ������������ĸ��������ĸ�
*/
JNIEXPORT jlong JNICALL Java_org_jwebtop_JWebTopNative_nCreateBrowser
(JNIEnv * env, jclass, jstring appFile, jlong parentWin
// ���²������滻appfile�е���Ӧ����
, jstring url       // Ҫ�򿪵����ӵ�ַ
, jstring title     // ��������
, jstring icon      // ����ͼ��
, jint x, jint y    // �������Ͻ�����,��ֵΪ-1ʱ�����ô˱���		 
, jint w, jint h    // ���ڵĿ��ߣ���ֵΪ-1ʱ�����ô˱���		
){
	wstringstream cmd;
	cmd << parentWin;
	cmd << " " << x << " " << y << " " << w << " " << h;// xywh��������null��ֻ������-1���������ȴ���
	wstring cmds = cmd.str();
	appendCMd(env, &cmds, appFile);
	appendCMd(env, &cmds, url);
	appendCMd(env, &cmds, icon);
	if (title != NULL){// title�����
		cmds.append(L" ").append(jstring2wstring(env, title).c_str());
	}
	wstring taskId = jw::task::createTaskId();			         // ��������id
	jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // �������������
	wstring wrapped = jw::wrapAsTaskJSON((long)g_RemoteWinHWnd, std::ref(taskId), std::ref(cmds));
	jw::sendProcessMsg(g_RemoteWinHWnd, JWM_CREATEBROWSER, LPTSTR(wrapped.c_str()));
	lock->wait();		             		 		             // �ȴ��������
	wstring result = lock->result;   		 		             // ȡ��ִ�н��
	return jw::parseLong(result);
}

/*
 * ��Ӧorg.jwebtop.JWebTopNative���nCloseBrowser����
 * �÷������ڹر�һ�����������
 * browserHWnd  ��������ھ��
 */
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nCloseBrowser
(JNIEnv * env, jclass, jlong browserHWnd){
	wstringstream wss; wss << browserHWnd;
	jw::sendProcessMsg((HWND)browserHWnd, JWM_CLOSEBROWSER, LPTSTR(wss.str().c_str()));
}

jstring exeRemoteAndWait(JNIEnv * env, jlong browserHWnd, string msg, DWORD msgId){
	wstring taskId = jw::task::createTaskId();			         // ��������id
	// taskId���ӵ�json�ַ�����
	wstring newmsg = jw::s2w(msg);
	wstring wrapped = jw::wrapAsTaskJSON((long)browserHWnd, std::ref(taskId), std::ref(newmsg));
	jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // �������������
	if (jw::sendProcessMsg((HWND)browserHWnd, msgId, LPTSTR(wrapped.c_str()))){ // ��������Զ�̽���
		lock->wait();		             		 		             // �ȴ��������
		wstring result = lock->result;   		 		             // ȡ��ִ�н��
		jstring sss = env->NewStringUTF(CefString(result).ToString().c_str());
		return sss;									 // ��������
	}
	else{
		jw::task::removeTask(taskId);								// ��Ϣ����ʧ���Ƴ�������Ϣ
		return NULL;											// �������ݣ�ע�������ǿ��ַ���
	}
}
JNIEXPORT jstring JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring script){
	return exeRemoteAndWait(env, browserHWnd, jstring2string(env,script), JWM_JS_EXECUTE_WAIT);
}

JNIEXPORT jstring JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSONWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring json){
	return exeRemoteAndWait(env, browserHWnd, jstring2string(env, json), JWM_JSON_EXECUTE_WAIT);
}

JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSNoWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring script){
	jw::sendProcessMsg((HWND)browserHWnd, JWM_JS_EXECUTE_RETURN, LPTSTR(jstring2wstring(env, script).c_str()));
}

// jni������ִ�нű��Ҳ��ȴ����ؽ��
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSONNoWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring json){
	jw::sendProcessMsg((HWND)browserHWnd, JWM_JSON_EXECUTE_RETURN, LPTSTR(jstring2wstring(env, json).c_str()));
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

// jni�������˳�JWebTop����
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExit
(JNIEnv *, jclass){
	jw::sendProcessMsg(g_RemoteWinHWnd, WM_COPYDATA_EXIT,L"");
}

JNIEXPORT jintArray Java_org_jwebtop_JWebTopNative_nGetWindowClient(JNIEnv * env, jclass, jlong hWnd){
	WINDOWINFO winInfo;
	GetWindowInfo((HWND)hWnd, &winInfo);// ��ȡ������Ϣ
	RECT rc = winInfo.rcClient;
	jintArray rtn= env->NewIntArray(4);
	jint * tmp = new jint[4];
	tmp[0] = rc.left;
	tmp[1] = rc.top;
	tmp[2] = rc.right;
	tmp[3] = rc.bottom;
	delete[] tmp;
	env->SetIntArrayRegion(rtn, 0, 4, tmp);
	return rtn;
}