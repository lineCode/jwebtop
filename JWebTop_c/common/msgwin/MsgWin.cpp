#include <Windows.h>
#include <string>
#include <sstream> 
#include <strsafe.h>
#include <thread>
#include "common/process/MultiProcess.h"
#include "common/util/StrUtil.h"
#include "common/JWebTopMsg.h"
#include "MsgWin.h"

using namespace std;
HWND g_LocalWinHWnd = NULL;  // ���ش�������Ϣ����HWND
HWND g_RemoteWinHWnd = NULL;  // Զ�̽��̵���Ϣ����HWND
namespace jw{
	// ����WM_COPYDATA��Ϣ
	LRESULT onWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
		wstring msg;
		DWORD msgId = 0;
		jw::parseProcessMsg(lParam, ref(msgId), ref(msg));
		if (msgId < MPMSG_USER){
			if (msgId == WM_COPYDATA_HWND){// Զ�̴��ڷ������䴰�ھ��ϵ��Ϣ
				long tmp = parseLong(msg);
				if (tmp != 0) g_RemoteWinHWnd = (HWND)tmp;
				return JWEBTOP_MSG_SUCCESS;
			}
			else if (msgId == WM_COPYDATA_EXIT){
				PostQuitMessage(0);
				return JWEBTOP_MSG_SUCCESS;
			}
		}
		std::thread t(msgwin_thread_executeWmCopyData, msgId, msg);// onWmCopyData��ͬ����Ϣ��Ϊ�˷�ֹ��һ���̵ĵȴ������������߳��н���ҵ����
		t.detach();// �ӵ�ǰ�̷߳���
		return JWEBTOP_MSG_SUCCESS;
	}

	// ������Ϣ����
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_COPYDATA:{
							 return	 onWmCopyData(hWnd, message, wParam, lParam);

		}
		case   WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	// ��������
	int createWin(HINSTANCE hInstance, LPTSTR szCmdLine){
		static TCHAR szAppName[] = TEXT("jwebtop_msgwin_cls");
		HWND hWnd;
		MSG msg;
		WNDCLASS wndclass;

		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = WndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		// ���� ͼ�깩����ʹ�� IDI-ͼʾID
		wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		// ���� ����깩����ʹ��  IDC-�α�ID
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		//
		wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = szAppName;

		// ע�ᴰ����
		if (!RegisterClass(&wndclass)){
			MessageBox(NULL, TEXT("�޷��������ڣ�����ϵͳ�汾���ͣ�"),
				szAppName, MB_ICONERROR);
			return 0;
		}

		hWnd = CreateWindow(szAppName, // window class name
			L"jwebtop_msgwin", // window caption
			WS_EX_TOOLWINDOW,// û�б߿�û�б����������пͻ����Ĵ���
			0, 0, 0, 0,// ��СΪ0�Ĵ���
			NULL, // parent window handle
			NULL, // window menu handle
			hInstance,//program instance handle
			NULL); // creation parameters
		g_LocalWinHWnd = hWnd;
		if (g_RemoteWinHWnd != NULL){// ���������Զ�̴��ڵľ������ô���䷢�͵�ǰ���ڵľ��
			wstringstream wss; wss << ((long)hWnd);
			sendProcessMsg(g_RemoteWinHWnd, WM_COPYDATA_HWND, LPTSTR(wss.str().c_str()));
		}
		onWindowHwndCreated(hWnd, szCmdLine);
		ShowWindow(hWnd, SW_HIDE);
		UpdateWindow(hWnd);
		while (GetMessage(&msg, NULL, 0, 0)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return msg.wParam;
	}
}