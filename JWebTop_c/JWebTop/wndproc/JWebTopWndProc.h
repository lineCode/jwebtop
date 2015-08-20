#ifndef CEF_JWEBTOP_WIN_WNDPROC_H_
#define CEF_JWEBTOP_WIN_WNDPROC_H_
#include <sstream>
#include <string>
#include "include/cef_browser.h"
#include "include/cef_resource_handler.h"
#include "include/cef_request.h"
#include "include/wrapper/cef_message_router.h"
#include "JWebTop/config/JWebTopConfigs.h"

using namespace std;
// ���ڵ��Թ��ߴ��ڵ�cefclient���������JWebTopHandler��Ѷ�JWebTop�Ĵ���������DEBUG��ҲӦ��һ�Σ�
class DEBUG_Handler : public CefClient{ IMPLEMENT_REFCOUNTING(DEBUG_Handler); };
class BrowserWindowInfo{
public:
	HWND hWnd;      // CEF��������ⲿ����
	HWND bWnd;      // CEF�ڲ����������
	//HWND msgWin;    // �û������ͨ�ŵ�Զ�̽��̴��ڣ�ֻ��ͨ��DLL����JWebTopʱ���д˲����� 

	LONG oldMainProc;                   // ���������������֮ǰ����Ϣ������
	LONG oldBrowserProc;                // ���������֮ǰ����Ϣ������
	CefRefPtr<CefBrowser> browser;      // ���ڹ������������
	//JWebTopConfigs  configs;            // ������ص�һЩ������Ϣ

	bool enableDrag;
	bool isDraging;                     // �Ƿ������϶�����
	LONG dragX, dragY;
};
BrowserWindowInfo * getBrowserWindowInfo(HWND hWnd);

typedef map<HWND, BrowserWindowInfo*> BrowserWindowInfoMap;// ����һ���洢BrowserWindowInfo��Map

// ��CEF��������ڵ���Ϣ��������
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// ����������Ϣ(configs)�Զ��㴰�ں�ʵ����������ڽ�������
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs * configs);


// 
namespace jb{
	void close(HWND hWnd);      // close(handler);// �رմ���

	void setWindowStyle(HWND hWnd, int exStyle);	//setWindowStyle(exStyle, handler);//�߼����������ô��ڶ������ԣ������ö�֮�ࡣ

	void loadUrl(HWND hWnd, wstring url);	//loadUrl(url, handler);//������ҳ��urlΪ��ҳ·��
	void reload(HWND hWnd);	//reload(handler);//���¼��ص�ǰҳ��
	void reloadIgnoreCache(HWND hWnd);	//reloadIgnoreCache(handler);//���¼��ص�ǰҳ�沢���Ի���
	void showDev(HWND hWnd);	//showDev(handler);//�򿪿����߹���

	void ExecJS(HWND hWnd, string js);
	void ExecJS(HWND hWnd, wstring js);

	void runApp(HWND hWnd, wstring appDefFile, long parentWin, const LPTSTR url, const LPTSTR &title, const LPTSTR &icon, const  int x, const  int y, const  int w, const  int h);

	// ��������Ľ��̲��ȴ�
	CefString invokeRemote_Wait(HWND hWnd, CefString json);
	void invokeRemote_NoWait(HWND hWnd, CefString json);

	bool sendJWebTopProcessMsg(HWND browserHWnd, DWORD msgId, LPTSTR msg);
}
#endif