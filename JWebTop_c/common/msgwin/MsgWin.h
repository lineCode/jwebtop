#ifndef jwebtop_multi_process_msgwin_H_
#define jwebtop_multi_process_msgwin_H_
#include <Windows.h>
namespace jw{
	// �������ڣ����ڴ�����������������ʱ���������̣�����Ӧ���ڶ����߳��е��ô˷���
	int createWin(HINSTANCE hInstance, LPTSTR szCmdLine);

	// createWin�������ڣ����ɴ��ڵľ��(HWND)�󣬻�ص��˷���
	void onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine);

	// ���ڽ���WM_COPYDATA��Ϣ�󣬻����µĶ����߳��е��ô˷���
	void msgwin_thread_executeWmCopyData(DWORD msgId, std::wstring msg, long senderHWnd, std::wstring taskId);
}
#endif