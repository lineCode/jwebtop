#ifndef CEF_JWEBTOP_EXE_DLLEX_H_
#define CEF_JWEBTOP_EXE_DLLEX_H_
#include <string>
using namespace std;
namespace jw{
	namespace dllex{// ��DLL���н����������չ����
		bool ex();
		void sendBrowserCreatedMessage(wstring taskId,long browserHWnd);
		// ����һ���߳���������Զ�̽����Ƿ���ֹ�Ա������ǰ����
		void waitForRemoteProcessTerminate();

		// ����������������ڵ�WM_COPYDATA��Ϣ
		LRESULT onBrowserWinWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		//LRESULT onWmCopyData(BrowserWindowInfo * bwInfo, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		//void thread_executeWmCopyData(HWND hWnd, DWORD msgId, std::wstring json);
	}
}
#endif