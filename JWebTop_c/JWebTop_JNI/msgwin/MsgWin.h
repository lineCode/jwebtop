#ifndef jwebtop_multi_process_msgwin_H_
#define jwebtop_multi_process_msgwin_H_
#include <Windows.h>

// ��������
int createWin(HINSTANCE hInstance, LPTSTR szCmdLine);

// ����createWin���лص�
void onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine);

// ����WM_COPYDATA��Ϣ
LRESULT onWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif