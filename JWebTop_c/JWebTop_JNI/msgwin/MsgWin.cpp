#include <Windows.h>
#include <string>
#include <sstream> 
#include <strsafe.h>

#include "MsgWin.h"

#define BTN_MAX 101
using namespace std;

// ������Ϣ����
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COPYDATA:{
		return	 onWmCopyData(hwnd, message, wParam, lParam);

	}
	case   WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

// ��������
int createWin(HINSTANCE hInstance, LPTSTR szCmdLine){
	static TCHAR szAppName[] = TEXT("jwebtop_msgwin_cls");
	HWND hwnd;
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
	
	hwnd = CreateWindow(szAppName, // window class name
		L"jwebtop_msgwin", // window caption
		WS_EX_TOOLWINDOW,// û�б߿�û�б����������пͻ����Ĵ���
		0, 0, 0, 0,// ��СΪ0�Ĵ���
		NULL, // parent window handle
		NULL, // window menu handle
		hInstance,//program instance handle
		NULL); // creation parameters

	onWindowHwndCreated(hwnd,szCmdLine);

	int  margin = 10, x = margin;
	CreateWindow(TEXT("button"),//����Ϊ��button    
		TEXT("���Ϳ������Ϣ"),//��ť����ʾ���ַ�    
		WS_CHILD | WS_VISIBLE, x, 10, 120, 25, hwnd,
		(HMENU)BTN_MAX, hInstance, NULL);

	ShowWindow(hwnd, SW_HIDE);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

