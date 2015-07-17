#ifndef CEF_JWEBTOP_WIN_CTRL_H_
#define CEF_JWEBTOP_WIN_CTRL_H_
#include "include\cef_client.h"
#include "JWebTopConfigs.h"

class BrowserWindowInfo{
public:
	HWND hWnd;
	HWND bWnd;

	LONG oldProc;                       // ����֮ǰ����Ϣ������
	CefRefPtr<CefBrowser> browser;      // ���ڹ������������
	JWebTopConfigs  configs;            // ������ص�һЩ������Ϣ

	bool isDraging;                     // �Ƿ������϶�����
	LONG dragX, dragY;
};
typedef std::map<HWND, BrowserWindowInfo*> BrowserWindowInfoMap;// ����һ���洢BrowserWindowInfo��Map

// ��CEF��������ڵ���Ϣ��������
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// ����������Ϣ(configs)�Զ��㴰�ں�ʵ����������ڽ�������
void renderBrowserWindow(CefRefPtr<CefBrowser> browser,JWebTopConfigs configs);

#endif