#include "jwebtop_dll.h"
#include <Shlwapi.h>
#include "common/task/Task.h"
#include "common/JWebTopMsg.h"
#include "common/util/StrUtil.h"
#include "common/winctrl/JWebTopWinCtrl.h"

#include "common/msgwin/MsgWin.h"
typedef BOOL(WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

extern HWND g_RemoteWinHWnd;
extern HWND g_LocalWinHWnd;
wstring processPath;

namespace jw{
	void msgwin_thread_executeWmCopyData(DWORD msgId, wstring json, long senderHWnd, std::wstring taskId){
		if (msgId == JWM_RESULT_RETURN){// Զ����������ɣ�����������ˣ���Ҫ֪ͨ�����̵ĵȴ��߳�ȥ��ȡ���
			jw::task::putTaskResult(taskId, json);						   // ֪ͨ�ȴ��̣߳�Զ����������ɣ������ȥȡ��
		}
		else{
			wstring result = invokeByBrowser(senderHWnd, json); 		         // ȡ��ִ�н��
			if (msgId == JWM_DLL_EXECUTE_WAIT){// Զ�̽��̷���һ�����񣬲���Զ�̽������ڵȴ���������ɺ���Ҫ����JWEBTOP_MSG_RESULT_RETURN��Ϣ��Զ�̽���
				sendProcessMsg((HWND)senderHWnd, JWM_RESULT_RETURN, result, (long)g_LocalWinHWnd, taskId); // ���ͽ����Զ�̽���
			}
		}
	}

	// ����createWin���лص�
	void onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine){
		wstringstream cmd;
		cmd << L": ";// ���Ϊ��������
		cmd << (long)hWnd << " ";// ׷��msgWin��HWND
		cmd << szCmdLine;// ���������в���
		createSubProcess(LPTSTR(processPath.c_str()), LPTSTR(cmd.str().c_str()));
	}
}

EXPORT void thread_CreateJWebTop(std::wstring cmds)
{
	jw::createWin(::GetModuleHandle(NULL), LPTSTR(cmds.c_str()));// createWin�Ὺ���µ��߳�
}

// ����JWebTop����
EXPORT long CreateJWebTop(wstring jprocessPath, wstring cfgFile){
	if (g_RemoteWinHWnd != NULL)return (long)g_RemoteWinHWnd;
	processPath = jprocessPath;
	wstring taskId = jw::task::createTaskId();			         // ��������id	
	wstring cmds(taskId);                                        // ������id�ŵ����������� 
	if (cfgFile[0] != L'\"')cfgFile = L"\"" + cfgFile + L"\"";   // ��˫���Ž�·����������
	cmds.append(L" ").append(cfgFile);	                         // �������������configFile
	jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // �������������
	thread t(thread_CreateJWebTop, cmds); t.detach();			 // �����߳��������������
	lock->wait();		             		 		             // �ȴ��������
	wstring result = lock->result;   		 		             // ȡ��ִ�н��
	long tmp = jw::parseLong(result);
	if (tmp != 0) g_RemoteWinHWnd = (HWND)tmp;
	return tmp;
}

/*
* �÷������ڴ���һ�����������
* jWebTopConfigJSON �����������ϢJSON
*/
EXPORT long CreateJWebTopBrowser(wstring jWebTopConfigJSON){
	wstring taskId = jw::task::createTaskId();			         // ��������id
	jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // �������������
	jw::sendProcessMsg(g_RemoteWinHWnd, JWM_CREATEBROWSER, jWebTopConfigJSON, (long)g_LocalWinHWnd, taskId);
	lock->wait();		             		 		             // �ȴ��������
	wstring result = lock->result;   		 		             // ȡ��ִ�н��
	return jw::parseLong(result);
}

/*
 * �÷������ڹر�һ�����������
 * browserHWnd  ��������ھ��
 */
EXPORT void CloseJWebTopBrowser(long browserHWnd){
	wstringstream wss; wss << browserHWnd;
	jw::sendProcessMsg((HWND)browserHWnd, JWM_CLOSEBROWSER, wss.str(), (long)g_LocalWinHWnd, wstring());
}

LPTSTR exeRemoteAndWait(long browserHWnd, wstring msg, DWORD msgId){
	wstring taskId = jw::task::createTaskId();			         // ��������id
	jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // �������������
	if (jw::sendProcessMsg((HWND)browserHWnd, msgId, msg, (long)g_LocalWinHWnd, taskId)){ // ��������Զ�̽���
		lock->wait();		             		 		             // �ȴ��������
		return LPTSTR(lock->result.c_str());   		 		             // ȡ��ִ�н��
	}
	else{
		jw::task::removeTask(taskId);								// ��Ϣ����ʧ���Ƴ�������Ϣ
		return NULL;											// �������ݣ�ע�������ǿ��ַ���
	}
}
EXPORT LPTSTR JWebTopExecuteJSWait(long browserHWnd, wstring script){
	return exeRemoteAndWait(browserHWnd, script, JWM_JS_EXECUTE_WAIT);
}

EXPORT LPTSTR JWebTopExecuteJSONWait(long browserHWnd, wstring json){
	return exeRemoteAndWait(browserHWnd, json, JWM_JSON_EXECUTE_WAIT);
}

void JWebTopExecuteJSNoWait(long browserHWnd, wstring script){
	jw::sendProcessMsg((HWND)browserHWnd, JWM_JS_EXECUTE_RETURN, script, (long)g_LocalWinHWnd, wstring());
}

// jni������ִ�нű��Ҳ��ȴ����ؽ��
void JWebTopExecuteJSONNoWait(long browserHWnd, wstring json){
	jw::sendProcessMsg((HWND)browserHWnd, JWM_JSON_EXECUTE_RETURN, json, (long)g_LocalWinHWnd, wstring());
}

// jni�������˳�JWebTop����
void ExitJWebTop(){
	jw::sendProcessMsg(g_RemoteWinHWnd, WM_COPYDATA_EXIT, wstring(), (long)g_LocalWinHWnd, wstring());
}