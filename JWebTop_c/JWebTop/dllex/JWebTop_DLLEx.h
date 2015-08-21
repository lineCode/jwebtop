#ifndef CEF_JWEBTOP_EXE_DLLEX_H_
#define CEF_JWEBTOP_EXE_DLLEX_H_
#include <string>
#include "include/wrapper/cef_message_router.h"
using namespace std;
namespace jw{
	namespace dllex{// ��DLL���н����������չ����
		// �Ƿ���dll��չ
		bool ex();
		
		// ����������Ѵ�����Ϣ���ͻ���
		void sendBrowserCreatedMessage(wstring taskId,long browserHWnd);

		// ����һ���߳���������Զ�̽����Ƿ���ֹ�Ա������ǰ����
		void waitForRemoteProcessTerminate();

		// ����������������ڵ�WM_COPYDATA��Ϣ
		LRESULT onBrowserWinWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		// js����dll�ķ��������ȴ����ؽ��
		CefString invokeRemote_Wait(HWND browserHWnd, CefString json);
		
		// js����dll�ķ����������ȴ����ؽ��
		void invokeRemote_NoWait(HWND browserHWnd, CefString json);

		// dll��ͬ����ʽ����js������js��ִ�н��ͨ����Ϣ��ʽ���͵�dll��
		void syncExecuteJS(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message);
	}
}
#endif