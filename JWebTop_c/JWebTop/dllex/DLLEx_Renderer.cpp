#include <windows.h>
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
#include "DLLExState.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif

//#define FLAG_TASK_TIMER 100
using namespace std;

namespace jw{
	namespace dllex{
		extern fastipc::Client * client;
		fastipc::Client * client_render;

		//bool ex(){ return client != NULL || client_render != NULL; }

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
			client->write(JWM_BROWSER_CREATED, browserHWnd, LPSTR(jw::w2s(taskId).c_str()), NULL, 0);
		}

		bool sendJWebTopProcessMsg(HWND browserHWnd, DWORD msgId, wstring& msg, wstring& taskId){
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
			BrowserWindowInfo * bw = getBrowserWindowInfo(browserHWnd);
			if (bw == NULL)writeLog(L"消息出现异常，sendJWebTopProcessMsg时bw==null,taskId"); writeLog(taskId); writeLog(L"\r\n");
#endif
			string tmp = jw::w2s(msg);
			if (client == NULL){// 如果是多进程模式，那么render会在自己的进程中，将服务器的信息记录到共享内存，以便render进程获取到相关信息
				static int step = 100;
				while (client_render == NULL){// 等待client_render创建成功				
					Sleep(step);
					step += step;
				}
				return client_render->write(msgId, (long)browserHWnd, LPSTR(jw::w2s(taskId).c_str()), LPSTR(tmp.c_str()), tmp.size()) == 0;
			} else{
				return client->write(msgId, (long)browserHWnd, LPSTR(jw::w2s(taskId).c_str()), LPSTR(tmp.c_str()), tmp.size()) == 0;
			}
		}

		void render_processMsg(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message){
			CefRefPtr<CefListValue> args = message->GetArgumentList();
			DWORD userMsgType = args->GetInt(0);
			if (userMsgType == JWM_B2R_TASKRESULT){
				wstring taskId = args->GetString(1).ToWString();
				wstring result = args->GetString(2).ToWString();
				jw::task::putTaskResult(taskId, result);
			}
			else if (userMsgType == JWM_B2R_SERVERINFO){
				DWORD blockSize = args->GetInt(1);
				wstring serverName = args->GetString(2).ToWString();
				client_render = new fastipc::Client();
				client_render->create(serverName, blockSize);
			}
			else{
				wstring taskInfo = args->GetString(1).ToWString();
				wstring remoteTaskId = args->GetString(2);
				//jw::task::putTaskResult(getFlagTaskId(remoteTaskId), L"1");// 通知等待任务，js已被正常接收
				//long remoteHWnd = args->GetInt(3);
				long browserHWnd = 0;
				CefString rtn;
				try{
					CefRefPtr<CefV8Context>  v8 = browser->GetMainFrame()->GetV8Context();
					if (v8 != NULL){
						int msgId = args->GetInt(0);
						if (v8->Enter()){
							CefRefPtr<CefV8Value> reval;
							CefRefPtr<CefV8Exception> exception;
							if (msgId == JWM_JSON_EXECUTE_WAIT){
								taskInfo = L"invokeByDLL(" + taskInfo + L")";// 包装json为js调用 
							}
							if (v8->Eval(taskInfo, reval, exception)){// 执行JS
								rtn = reval->GetStringValue();
							}
							CefRefPtr<CefV8Value> global = v8->GetGlobal();
							CefRefPtr<CefV8Value> jwebtop = global->GetValue(CefString("JWebTop"));
							CefRefPtr<CefV8Value> handler = jwebtop->GetValue("handler");
							browserHWnd = handler->GetIntValue();// 从JWebTop.handler获取窗口句柄
							v8->Exit();
						}
					}
				}
				catch (...){}// 不管发生什么情况，都需要把taskId返回回去，否则会造成系统在那里死死的等待
				wstring result = rtn.ToWString();
				sendJWebTopProcessMsg((HWND)browserHWnd, JWM_RESULT_RETURN, result, remoteTaskId); // 发送结果到远程进程
			}
		}

		CefString invokeRemote_Wait(HWND browserHWnd, CefString json){
			//BrowserWindowInfo * bwInfo = getBrowserWindowInfo(browserHWnd);
			//if (bwInfo == NULL)return CefString();
			wstring taskId = jw::task::createTaskId();			         // 生成任务id
			wstring newjson = json.ToWString();
			jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // 放置任务到任务池		
			if (sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_WAIT, newjson, taskId)){ // 发送任务到远程进程
				DLLExState::findOrCreateExState(browserHWnd)->addLockedTask(lock);							// 记录当前浏览器已经创建的锁
				wstring result = lock->wait();   	 						// 等待任务完成并取回执行结果
				DLLExState::findOrCreateExState(browserHWnd)->removeLockedTask(lock);						// 移除当前浏览器已经解开的锁
				delete lock;
				return CefString(result);									// 返回数据
			}
			else{
				jw::task::removeTask(taskId);								// 消息发送失败移除现有消息
				return CefString();											// 返回数据：注意这里是空字符串
			}
		}

		void invokeRemote_NoWait(HWND browserHWnd, CefString json){
			wstring newjson = json.ToWString(), empty;
			sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_RETURN, newjson, empty); // 发送任务到远程进程
		}
	}// End dllex namespace
}// End jw namespace