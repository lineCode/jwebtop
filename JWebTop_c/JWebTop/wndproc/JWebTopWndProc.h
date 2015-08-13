#ifndef CEF_JWEBTOP_WIN_WNDPROC_H_
#define CEF_JWEBTOP_WIN_WNDPROC_H_
#include <sstream>
#include <string>
#include "include/cef_browser.h"
#include "include/cef_resource_handler.h"
#include "include/cef_request.h"
#include "include/wrapper/cef_message_router.h"
#include "JWebTop/config/JWebTopConfigs.h"

// ���ڵ��Թ��ߴ��ڵ�cefclient���������JWebTopHandler��Ѷ�JWebTop�Ĵ���������DEBUG��ҲӦ��һ�Σ�
class DEBUG_Handler : public CefClient{ IMPLEMENT_REFCOUNTING(DEBUG_Handler); };
class BrowserWindowInfo{
public:
	HWND hWnd;      // CEF��������ⲿ����
	HWND bWnd;      // CEF�ڲ����������
	HWND msgWin;    // �û������ͨ�ŵ�Զ�̽��̴��ڣ�ֻ��ͨ��DLL����JWebTopʱ���д˲����� 

	LONG oldMainProc;                   // ���������������֮ǰ����Ϣ������
	LONG oldBrowserProc;                // ���������֮ǰ����Ϣ������
	CefRefPtr<CefBrowser> browser;      // ���ڹ������������
	JWebTopConfigs  configs;            // ������ص�һЩ������Ϣ

	bool isDraging;                     // �Ƿ������϶�����
	LONG dragX, dragY;
};
typedef std::map<HWND, BrowserWindowInfo*> BrowserWindowInfoMap;// ����һ���洢BrowserWindowInfo��Map
// ����WM_COPYDATA��Ϣ
LRESULT onWmCopyData(BrowserWindowInfo * bwInfo, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void thread_executeWmCopyData(HWND hWnd, DWORD msgId, std::wstring json);


// ��CEF��������ڵ���Ϣ��������
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// ����������Ϣ(configs)�Զ��㴰�ں�ʵ����������ڽ�������
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs * configs);


// 
namespace jb{
	void close(HWND hWnd);      // close(handler);// �رմ���

	void setWindowStyle(HWND hWnd, int exStyle);	//setWindowStyle(exStyle, handler);//�߼����������ô��ڶ������ԣ������ö�֮�ࡣ
	
	void loadUrl(HWND hWnd, std::wstring url);	//loadUrl(url, handler);//������ҳ��urlΪ��ҳ·��
	void reload(HWND hWnd);	//reload(handler);//���¼��ص�ǰҳ��
	void reloadIgnoreCache(HWND hWnd);	//reloadIgnoreCache(handler);//���¼��ص�ǰҳ�沢���Ի���
	void showDev(HWND hWnd);	//showDev(handler);//�򿪿����߹���

	void ExecJS(HWND hWnd, std::string js);
	void ExecJS(HWND hWnd, std::wstring js);

	void runApp(HWND hWnd, std::wstring appDefFile, long parentWin);


	// ��������Ľ��̲��ȴ�
	CefString invokeRemote_Wait(HWND hWnd, CefString json);
	void invokeRemote_NoWait(HWND hWnd, CefString json);
}
#endif