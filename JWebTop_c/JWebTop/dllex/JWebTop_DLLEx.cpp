#include <windows.h>
#include <ShellAPI.h> 
#include <thread>
#include <sstream>
#include "JWebTop_DLLEx.h"
#include "common/util/StrUtil.h"
#include "common/JWebTopMsg.h"
#include "common/task/Task.h"
#include "common/msgwin/MsgWin.h"
#include "JWebTop/config/JWebTopConfigs.h"
#include "JWebTop/browser/JWebTopClient.h"
#include "JWebTop/browser/JWebTopCommons.h"
#include "JWebTop/wndproc/JWebTopWndProc.h"

#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif

using namespace std;
extern HWND g_LocalWinHWnd;  // ���ش�������Ϣ����HWND
extern HWND g_RemoteWinHWnd;  // Զ�̽��̵���Ϣ����HWND
namespace jw{
	//wstring g_TaskId;                 // ������Ϣ���ڻ������ʱ������id
	namespace dllex{
		bool ex(){ return g_RemoteWinHWnd != NULL; }

		void waitRemoteProcessTerminate(){
			try{
				DWORD lpdwProcessId;
				GetWindowThreadProcessId(g_RemoteWinHWnd, &lpdwProcessId);
				//HANDLE hHandle = GetProcessHandleFromHwnd(g_RemoteWinHWnd);// �˺�����ͷ�ļ���ȷ�����ĸ�(Header: Use LoadLibrary and GetProcAddress.  Library: Use Oleacc.dll.)
				HANDLE hHandle = OpenProcess(PROCESS_ALL_ACCESS, false, lpdwProcessId);
				WaitForSingleObject(hHandle, INFINITE);
			}
			catch (...){
			}
			jw::sendProcessMsg(g_LocalWinHWnd, WM_COPYDATA_EXIT, L"");// ֪ͨ�����������ڣ�������Ҫ�ر�
		}
		// ����һ���߳���������Զ�̽����Ƿ���ֹ�Ա������ǰ����
		void waitForRemoteProcessTerminate(){
			std::thread waitRemoteProcessThread(waitRemoteProcessTerminate);
			waitRemoteProcessThread.detach();// �ڶ����߳��еȴ�Զ�̽��̵Ľ�����Ϣ
		}

		void browserwin_thread_executeWmCopyData(HWND browserHWnd, DWORD msgId, wstring msg){
			BrowserWindowInfo * bwInfo = getBrowserWindowInfo(browserHWnd);
			switch (msgId)
			{
			case JWM_RESULT_RETURN:// Զ����������ɣ�����������ˣ���Ҫ֪ͨ�����̵ĵȴ��߳�ȥ��ȡ���
			{
									   wstring taskId, result;
									   long remoteHWnd;
									   jw::parseMessageJSON(msg, ref(remoteHWnd), ref(taskId), ref(result));  // ��������Ϣ�н���������id����������
									   jw::task::putTaskResult(taskId, result);	   					   // ֪ͨ�ȴ��̣߳�Զ����������ɣ������ȥȡ��
									   break;
			}
			case JWM_JSON_EXECUTE_WAIT:// Զ�̽��̷���һ�����񣬲���Զ�̽������ڵȴ���������ɺ���Ҫ����JWEBTOP_MSG_RESULT_RETURN��Ϣ��Զ�̽���
			case JWM_JS_EXECUTE_WAIT:// Զ�̽��̷���һ�����񣬲���Զ�̽������ڵȴ���������ɺ���Ҫ����JWEBTOP_MSG_RESULT_RETURN��Ϣ��Զ�̽���
			{

										 CefRefPtr<CefProcessMessage> cefMsg = CefProcessMessage::Create("waitjs");
										 CefRefPtr<CefListValue> args = cefMsg->GetArgumentList();
										 args->SetInt(0, msgId);
										 args->SetString(1, msg);
										 bwInfo->browser->SendProcessMessage(PID_RENDERER, cefMsg);// ֱ�ӷ��͵�render����ȥִ��
										 break;
			}
			case JWM_JSON_EXECUTE_RETURN:
				msg = L"invokeByDLL(" + msg + L")";// ��װjsonΪjs���� 
				bwInfo->browser->GetMainFrame()->ExecuteJavaScript(msg, "", 0);
				break;
			case JWM_JS_EXECUTE_RETURN:
				bwInfo->browser->GetMainFrame()->ExecuteJavaScript(msg, "", 0);
				break;
			case JWM_CLOSEBROWSER:
				jb::close((HWND)parseLong(msg));
				break;
			default:
				break;
			}
		}
		LRESULT onBrowserWinWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
			wstring msg;
			DWORD msgId = 0;
			jw::parseProcessMsg(lParam, ref(msgId), ref(msg));
			std::thread t(browserwin_thread_executeWmCopyData, hWnd, msgId, msg);// onWmCopyData��ͬ����Ϣ��Ϊ�˷�ֹ��һ���̵ĵȴ������������߳��н���ҵ����
			t.detach();// �ӵ�ǰ�̷߳���
			return JWEBTOP_MSG_SUCCESS;
		}

		void sendBrowserCreatedMessage(wstring taskId, long browserHWnd){
			wstringstream wss; wss << browserHWnd;
			wstring ws = wss.str();
			wstring wrapped = jw::wrapAsTaskJSON((long)g_RemoteWinHWnd, std::ref(taskId), std::ref(ws));
			sendProcessMsg(g_RemoteWinHWnd, JWM_RESULT_RETURN, LPTSTR(wrapped.c_str()));
		}

		bool sendJWebTopProcessMsg(HWND browserHWnd, DWORD msgId, LPTSTR msg){
			BrowserWindowInfo * bw = getBrowserWindowInfo(browserHWnd);
			if (bw == NULL)return false;
			if (g_RemoteWinHWnd == NULL)return false;
			return jw::sendProcessMsg(g_RemoteWinHWnd, msgId, msg);
		}

		void syncExecuteJS(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message){
			CefRefPtr<CefListValue> args = message->GetArgumentList();
			CefString msg = args->GetString(1);
			wstring remoteTaskId, taskInfo;
			long remoteHWnd;
			jw::parseMessageJSON(msg, ref(remoteHWnd), ref(remoteTaskId), ref(taskInfo));  // ��������Ϣ�н���������id����������
			CefString rtn;
			CefRefPtr<CefV8Context>  v8 = browser->GetMainFrame()->GetV8Context();
			long browserHWnd = 0;
			if (v8 != NULL){
				int msgId = args->GetInt(0);
				if (v8->Enter()){
					CefRefPtr<CefV8Value> reval;
					CefRefPtr<CefV8Exception> exception;
					if (msgId == JWM_JSON_EXECUTE_WAIT){
						taskInfo = L"invokeByDLL(" + taskInfo + L")";// ��װjsonΪjs���� 
					}
					//taskInfo = L"testInvoke()";// FIXME��������
					if (v8->Eval(taskInfo, reval, exception)){// ִ��JS
						rtn = reval->GetStringValue();
					}
					CefRefPtr<CefV8Value> global = v8->GetGlobal();
					CefRefPtr<CefV8Value> jwebtop = global->GetValue(CefString("JWebTop"));
					CefRefPtr<CefV8Value> handler = jwebtop->GetValue("handler");
					browserHWnd = handler->GetIntValue();// ��JWebTop.handler��ȡ���ھ��
					v8->Exit();
				}
			}
			wstring result = rtn.ToWString();
			wstring wrapped = jw::wrapAsTaskJSON(browserHWnd, std::ref(remoteTaskId), std::ref(result)); // ��װ�������
			//FIXME�������ʱ����ķ�����ʧ�ܣ���Ϊ��ȡ����browserHWnd��Ӧ��BrowserWindowInfo
			sendJWebTopProcessMsg((HWND)browserHWnd, JWM_RESULT_RETURN, LPTSTR(wrapped.c_str())); // ���ͽ����Զ�̽���
		}

		CefString invokeRemote_Wait(HWND browserHWnd, CefString json){
			wstring taskId = jw::task::createTaskId();			         // ��������id
			// taskId���ӵ�json�ַ�����
			wstring newjson = json.ToWString();
			wstring wrapped = jw::wrapAsTaskJSON((long)browserHWnd, std::ref(taskId), std::ref(newjson));
			jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // �������������
			if (sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_WAIT, LPTSTR(wrapped.c_str()))){ // ��������Զ�̽���
				lock->wait();		             		 		             // �ȴ��������
				wstring result = lock->result;   		 		             // ȡ��ִ�н��
				return CefString(result);									 // ��������
			}
			else{
				jw::task::removeTask(taskId);								// ��Ϣ����ʧ���Ƴ�������Ϣ
				return CefString();											// �������ݣ�ע�������ǿ��ַ���
			}
		}

		void invokeRemote_NoWait(HWND browserHWnd, CefString json){
			sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_RETURN, LPTSTR(json.ToWString().c_str())); // ��������Զ�̽���
		}
	}

	// ������Ϣ���ڵ�WM_COPYDATA��Ϣ
	void msgwin_thread_executeWmCopyData(DWORD msgId, std::wstring msg){
		if (msgId == JWM_CREATEBROWSER){
			wstring taskId, result;
			long remoteHWnd;
			jw::parseMessageJSON(msg, ref(remoteHWnd), ref(taskId), ref(result));  // ��������Ϣ�н���������id����������
			createNewBrowser(JWebTopConfigs::parseCreateBrowserCmdLine(result),taskId);
		}
		// ��ʱ����Ҫ�����κδ���
	}
	// ����createWin���лص�
	void onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine){
		if (g_RemoteWinHWnd != NULL){// ���������Զ�̴��ڵľ������ô���䷢�͵�ǰ���ڵľ��
			wstring taskId = wstring(szCmdLine);
			wstringstream wss; wss << ((long)hWnd);
			wstring ws = wss.str();
			wstring wrapped = jw::wrapAsTaskJSON((long)g_RemoteWinHWnd, std::ref(taskId), std::ref(ws));
			sendProcessMsg(g_RemoteWinHWnd, JWM_RESULT_RETURN, LPTSTR(wrapped.c_str()));
		}
	}
}