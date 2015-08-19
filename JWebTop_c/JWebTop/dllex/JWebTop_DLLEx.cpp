#include <windows.h>
#include <thread>
#include "JWebTop_DLLEx.h"
#include "common/util/StrUtil.h"
#include "JWebTop/config/JWebTopConfigs.h"
#include "common/JWebTopMsg.h"
#include "common/task/Task.h"
#include "common/msgwin/MsgWin.h"
#include "JWebTop/browser/JWebTopClient.h"

extern HWND g_LocalWinHWnd;  // ���ش�������Ϣ����HWND
extern HWND g_RemoteWinHWnd;  // Զ�̽��̵���Ϣ����HWND

namespace jw{
	namespace dllex{
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
	}
	// ������Ϣ���ڵ�WM_COPYDATA��Ϣ
	void msgwin_thread_executeWmCopyData(DWORD msgId, std::wstring msg){
		// ��ʱ����Ҫ���κδ���
	}
	// ����createWin���лص�
	void onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine){
		// ��ʱ����Ҫ���κδ���
	}

}