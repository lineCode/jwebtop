#ifndef CEF_JWEBTOP_WIN_CTRL_H_
#define CEF_JWEBTOP_WIN_CTRL_H_
#include <sstream>
#include <string>
//#include "include\cef_client.h"
#include "include/cef_browser.h"
#include "include/cef_resource_handler.h"
#include "include/cef_request.h"
#include "include/wrapper/cef_message_router.h"
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
// 
namespace jw{
	void setTitle(HWND hWnd, std::wstring title);
	std::wstring getTitle(HWND hWnd);
	void move(HWND hWnd, int x, int y);
	void setSize(HWND hWnd, int w, int h);
	void setBound(HWND hWnd, int x, int y, int w, int h);
}
#endif