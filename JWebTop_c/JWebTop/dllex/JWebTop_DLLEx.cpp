#include <windows.h>
#include <ShellAPI.h> 
#include <thread>
#include <sstream>
#include "JWebTop_DLLEx.h"
#include "common/util/StrUtil.h"
#include "common/JWebTopMsg.h"
#include "common/task/Task.h"
#include "JWebTop/config/JWebTopConfigs.h"
#include "JWebTop/browser/JWebTopClient.h"
#include "JWebTop/browser/JWebTopCommons.h"
#include "JWebTop/browser/JWebTopContext.h"
#include "JWebTop/wndproc/JWebTopWndProc.h"
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "include/cef_app.h"

#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif

using namespace std;
namespace jw{
	namespace dllex{
		bool isEx = false;

		fastipc::Server server;
		fastipc::Client client;

		bool ex(){ return isEx; }
		void setAsEx(){ isEx = true; }

		void closeWebTopEx();

		void __onRead(LONG userMsgType, LONG userValue, std::string taskIds, std::string datas){
			std::wstring taskId = jw::s2w(taskIds);
			std::wstring data = jw::s2w(datas);
			switch (userMsgType){
			case JWM_RESULT_RETURN:
				jw::task::putTaskResult(taskId, data); // ֪ͨ�ȴ��̣߳�Զ����������ɣ������ȡ��
				return;
			case JWM_JSON_EXECUTE_WAIT:// Զ�̽��̷���һ�����񣬲���Զ�̽������ڵȴ���������ɺ���Ҫ����JWEBTOP_MSG_RESULT_RETURN��Ϣ��Զ�̽���
			case JWM_JS_EXECUTE_WAIT:// Զ�̽��̷���һ�����񣬲���Զ�̽������ڵȴ���������ɺ���Ҫ����JWEBTOP_MSG_RESULT_RETURN��Ϣ��Զ�̽���
			{
										 BrowserWindowInfo * bwInfo = getBrowserWindowInfo((HWND)userValue);
										 CefRefPtr<CefProcessMessage> cefMsg = CefProcessMessage::Create("waitjs");
										 CefRefPtr<CefListValue> args = cefMsg->GetArgumentList();
										 args->SetInt(0, userMsgType);
										 args->SetString(1, data);
										 args->SetString(2, taskId);
										 bwInfo->browser->SendProcessMessage(PID_RENDERER, cefMsg);// ֱ�ӷ��͵�render����ȥִ��
										 break;
			}
			case JWM_JSON_EXECUTE_RETURN:{
											 BrowserWindowInfo * bwInfo = getBrowserWindowInfo((HWND)userValue);
											 data = L"invokeByDLL(" + data + L")";// ��װjsonΪjs���� 
											 bwInfo->browser->GetMainFrame()->ExecuteJavaScript(data, "", 0);
											 break; }
			case JWM_JS_EXECUTE_RETURN:{
										   BrowserWindowInfo * bwInfo = getBrowserWindowInfo((HWND)userValue);
										   bwInfo->browser->GetMainFrame()->ExecuteJavaScript(data, "", 0);
										   break;
			}
			case JWM_CLOSEBROWSER:
				jb::close((HWND)userValue);
			case JWM_CREATEBROWSER_JSON:
				createNewBrowser(JWebTopConfigs::loadAsJSON(data), taskId);
				return;
			case JWM_CREATEBROWSER_FILE:
				createNewBrowser(JWebTopConfigs::loadConfigs(data), taskId);
				return;
			case JWM_CFGJWEBTOP_FILE:
				jw::ctx::startJWebTopByFile(data);
				return;
			case JWM_CEF_ExitAPP:
				closeWebTopEx();
			}
		}

		class EXEReadListener :public fastipc::RebuildedBlockListener{
			void onRebuildedRead(fastipc::MemBlock* memBlock){
#ifdef JWebTopLog
				std::wstringstream wss;
				wss << L"Readed "
					<< L" userMsgType=" << memBlock->userMsgType
					<< L" userValue=" << memBlock->userValue
					<< L" userShortStr=" << jw::s2w(memBlock->getUserShortStr())
					<< L" pBuff=" << jw::s2w(memBlock->getData())
					<< L"||\r\n";
				writeLog(wss.str());
#endif
				thread t(__onRead, memBlock->userMsgType, memBlock->userValue, memBlock->getUserShortStr(), memBlock->getData());
				t.detach();// ��һ�������߳���������������������������ݵ��շ�
			}
		};

		void __startIPCServer(){
			server.startRead();
		}
		void __waitProcess(DWORD processId){
			// �ȴ�Զ�̽��̽���
			try{
				HANDLE hHandle = OpenProcess(SYNCHRONIZE, false, processId);// ����Ȩ�ޣ�������Щ�����OpendProcessʧ�ܣ�����xp��
				WaitForSingleObject(hHandle, INFINITE);// �ȴ�Զ�̽��̽���
				closeWebTopEx();
			}
			catch (...){
#ifdef JWebTopLog
				writeLog("�ȴ�Զ�̽��̽���������������\r\n");
#endif
			}
		}
		int startIPCServer(wstring &serverName, DWORD blockSize, DWORD processId){
			int r = client.create(serverName, blockSize);
			if (r != 0)return r;
			r = server.create(fastipc::genServerName(serverName), blockSize);
			if (r != 0)return r;
			server.setListener(new EXEReadListener());// ����������
			thread startServer(__startIPCServer);
			startServer.detach();								// �����߳�������exe�˵�FastIPC����
			client.write(JWM_IPC_CLIENT_OK, 0, NULL, NULL, 0);	// ֪ͨdll�ˣ��ӽ����Ѵ��������Խ���������ĳ�ʼ����
			thread waitProcess(__waitProcess, processId);		// �����߳��������ȴ�Զ�̽��̽����ļ���
			waitProcess.detach();
			return 0;
		}
		void closeWebTopEx(){
			isEx = false;					// ���Ϊ��exģʽ���Ա��˳�cef����
			jw::ctx::CloseAllBrowsers(true);// �ر����������
			server.close();					// �ر�ipc����
		}
		void OnContextInitialized(){
			client.write(JWM_CEF_APP_INITED, 0, NULL, NULL, 0); // CEF������ѳ�ʼ���
		}

		void sendBrowserCreatedMessage(wstring taskId, long browserHWnd){
#ifdef JWebTopLog
			std::wstringstream wss;
			wss << L"Writed "
				<< L" sendBrowserCreatedMessage"
				<< L" taskId=" << taskId
				<< L" browserHWnd=" << browserHWnd
				<< L"||\r\n";
			writeLog(wss.str());
#endif
			client.write(JWM_BROWSER_CREATED, browserHWnd, LPSTR(jw::w2s(taskId).c_str()), NULL, 0);
		}

		bool sendJWebTopProcessMsg(HWND browserHWnd, DWORD msgId, wstring msg, long senderHWND, wstring taskId){
			BrowserWindowInfo * bw = getBrowserWindowInfo(browserHWnd);
			if (bw == NULL)return false;
#ifdef JWebTopLog
			std::wstringstream wss;
			wss << L"Writed "
				<< L" sendJWebTopProcessMsg"
				<< L" taskId=" << taskId
				<< L" browserHWnd=" << browserHWnd
				<< L" msgId=" << msgId
				<< L" msg=" << msg
				<< L"||\r\n";
			writeLog(wss.str());
#endif
			string tmp = jw::w2s(msg);
			return client.write(msgId, (long)browserHWnd, LPSTR(jw::w2s(taskId).c_str()), LPSTR(tmp.c_str()), tmp.size()) == 0;
		}

		void syncExecuteJS(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message){
			CefRefPtr<CefListValue> args = message->GetArgumentList();
			wstring taskInfo = args->GetString(1).ToWString();
			wstring remoteTaskId = args->GetString(2);
			//long remoteHWnd = args->GetInt(3);
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
			sendJWebTopProcessMsg((HWND)browserHWnd, JWM_RESULT_RETURN, result, (long)browserHWnd, remoteTaskId); // ���ͽ����Զ�̽���
		}

		CefString invokeRemote_Wait(HWND browserHWnd, CefString json){
			wstring taskId = jw::task::createTaskId();			         // ��������id
			wstring newjson = json.ToWString();
			jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // �������������
			if (sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_WAIT, newjson, (long)browserHWnd, taskId)){ // ��������Զ�̽���
				wstring result = lock->wait();     		 		             // �ȴ�������ɲ�ȡ��ִ�н��
				return CefString(result);									 // ��������
			}
			else{
				jw::task::removeTask(taskId);								// ��Ϣ����ʧ���Ƴ�������Ϣ
				return CefString();											// �������ݣ�ע�������ǿ��ַ���
			}
		}

		void invokeRemote_NoWait(HWND browserHWnd, CefString json){
			wstring newjson = json.ToWString();
			sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_RETURN, newjson, (long)browserHWnd, wstring()); // ��������Զ�̽���
		}
	}
}