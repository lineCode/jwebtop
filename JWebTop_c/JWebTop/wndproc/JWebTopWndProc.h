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
	HWND hWnd;
	HWND bWnd;

	LONG oldMainProc;                   // ���������������֮ǰ����Ϣ������
	LONG oldBrowserProc;                // ���������֮ǰ����Ϣ������
	CefRefPtr<CefBrowser> browser;      // ���ڹ������������
	JWebTopConfigs  configs;            // ������ص�һЩ������Ϣ

	bool isDraging;                     // �Ƿ������϶�����
	LONG dragX, dragY;
};
typedef std::map<HWND, BrowserWindowInfo*> BrowserWindowInfoMap;// ����һ���洢BrowserWindowInfo��Map

// ��CEF��������ڵ���Ϣ��������
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// ����������Ϣ(configs)�Զ��㴰�ں�ʵ����������ڽ�������
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs * configs);
// 
namespace jb{
	//void setCenter(HWND hWnd);                             // setToCenter(handler);// �ô�����ʾ����Ļ����
	//void setCenter(HWND hWnd, int x, int y, int w, int h); // setToCenter(handler);// �ô�����ʾ����Ļ����
	//POINT getPos(HWND hWnd);                               // getPos(handler);//��ô���λ�ã�����ֵΪһobject����ʽ����{x:13,y:54}
	//void setSize(HWND hWnd, int w, int h);                 // setSize(x, y, handler);//���ô��ڴ�С
	//POINT getSize(HWND hWnd);                              // getSize(handler);//��ô��ڴ�С������ֵΪһobject����ʽ����{width:130,height:54}
	//POINT getScreenSize();                                 // getScreenSize();//��ȡ��Ļ��С������ֵΪһobject����ʽ����{width:130,height:54}
	//void move(HWND hWnd, int x, int y);                    // move(x, y, handler);//�ƶ�����
	//void setBound(HWND hWnd, int x, int y, int w, int h);  // setBound(x, y,w ,h, handler);// ͬʱ���ô��ڵ�����ʹ�С
	//RECT getBound(HWND hWnd);                              // getBound(handler);//��ȡ���ڵ�λ�úʹ�С������ֵΪһobject����ʽ����{x:100,y:100,width:130,height:54}
	//void setTitle(HWND hWnd, std::wstring title);          // setTitle(title, handler);// ���ô�������
	//std::wstring getTitle(HWND hWnd);                      // getTitle(handler);// ��ȡ�������ƣ�����ֵΪһ�ַ���

	//void bringToTop(HWND hWnd);	// bringToTop(handler);//�����Ƶ����
	//void focus(HWND hWnd);	    // focus(handler);//ʹ���ڻ�ý���
	//void hide(HWND hWnd);	    // hide(handler);//���ش���
	//void max(HWND hWnd);	    // max(handler);//��󻯴���
	//void mini(HWND hWnd);	    // mini(hander);//��С������
	//void restore(HWND hWnd);	// restore(handler);//��ԭ���ڣ���Ӧ��hide����
	//void setTopMost(HWND hWnd);	// setTopMost(handler);//�����ö����˺�����bringToTop���������ڴ˺�����ʹ������Զ�ö�������������һ�����ڵ������ö�����

	void close(HWND hWnd);      // close(handler);// �رմ���

	void setWindowStyle(HWND hWnd, int exStyle);	//setWindowStyle(exStyle, handler);//�߼����������ô��ڶ������ԣ������ö�֮�ࡣ
	
	void loadUrl(HWND hWnd, std::wstring url);	//loadUrl(url, handler);//������ҳ��urlΪ��ҳ·��
	void reload(HWND hWnd);	//reload(handler);//���¼��ص�ǰҳ��
	void reloadIgnoreCache(HWND hWnd);	//reloadIgnoreCache(handler);//���¼��ص�ǰҳ�沢���Ի���
	void showDev(HWND hWnd);	//showDev(handler);//�򿪿����߹���

	void ExecJS(HWND hWnd, std::string js);
	void ExecJS(HWND hWnd, std::wstring js);

	void runApp(std::wstring appDefFile, long parentWin);

#ifdef JWebTopJNI // ֻ����JWebTop_JNI��Ŀ�£�����Ĵ���Ż����
	CefString invokeJavaMethod(CefString json);// ��;����JS����Java���롣�÷���invokeJava(jsonstring);
#endif
}
#endif