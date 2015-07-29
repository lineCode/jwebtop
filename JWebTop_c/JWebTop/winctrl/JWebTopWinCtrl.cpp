#include "JWebTopWinCtrl.h"

#include "JWebTop/browser/JWebTopCommons.h"
#include "JWebTop/util/StrUtil.h"
#include "JWebTop/tests/TestUtil.h"
using namespace std;
BrowserWindowInfoMap BrowserWindowInfos;// �ھ�̬�����л��������Ѵ����Ĵ�����Ϣ

HICON GetIcon(CefString url, CefString path){
	if (path.ToWString().find(L":") == -1){// ���ָ����·�������·��
		wstring _path;
		_path = url.ToWString();
		replace_allW(_path, L"\\", L"/");
		_path = _path.substr(0, _path.find_last_of('/') + 1);
		path = _path.append(path);
	}
	return (HICON)::LoadImage(NULL, path.ToWString().data(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
}

BrowserWindowInfo * getBrowserWindowInfo(HWND hWnd){
	BrowserWindowInfoMap::iterator it = BrowserWindowInfos.find(hWnd);
	if (BrowserWindowInfos.end() != it) {
		return it->second;
	}
	return NULL;
}
// ���������ڵ���Ϣ����Щ��Ϣ������رմ��ڡ��ƶ����ڵȣ�ֻ���������ڲ���������
LRESULT CALLBACK JWebTop_WindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	// ���ԶԱ�Ҫ����Ϣ����Ԥ�ȴ�����Ҫ���ص���Ϣ�Ϳ��Բ��÷����������stringstream s;
	switch (message) {
	case WM_SIZE:
	{
					stringstream js_event;
					js_event << "var e = new CustomEvent(' ',{"// AlloyDesktopWindowResize
						<< "	detail:{"
						<< "		width:" << LOWORD(lParam) << ","
						<< "		height:" << HIWORD(lParam)
						<< "	}"
						<< "});"
						<< "dispatchEvent(e);";
					bwInfo->browser->GetMainFrame()->ExecuteJavaScript(js_event.str(), "", 0);
					break;
	}// End case-WM_SIZE
	case WM_MOVE:
	{
					stringstream js_event;
					js_event << "var e = new CustomEvent('JWebTopMove',{"// AlloyDesktopWindowMove
						<< "	detail:{"
						<< "		x:" << LOWORD(lParam) << ","
						<< "		y:" << HIWORD(lParam)
						<< "	}"
						<< "});"
						<< "dispatchEvent(e);";
					bwInfo->browser->GetMainFrame()->ExecuteJavaScript(js_event.str(), "", 0);
					break;
	}// End case-WM_MOVE
	case WM_ACTIVATE:
	{
						stringstream js_event;
						js_event << "var e = new CustomEvent('JWebTopWindowActive',{"// AlloyDesktopWindowActive
							<< "	detail:{"
							<< "		handler:0x" << bwInfo->hWnd
							<< "       ,w:'main'"
							<< "	}"
							<< "});"
							<< "dispatchEvent(e);";
						bwInfo->browser->GetMainFrame()->ExecuteJavaScript(js_event.str(), "", 0);
						break;
	}// End case-WM_ACTIVATE
	case WM_ACTIVATEAPP:
	{
						   stringstream js_event;
						   js_event << "var e = new CustomEvent('JWebTopActive',{"
							   << "	detail:{"
							   << "		handler:0x" << bwInfo->hWnd
							   << "       ,w:'main'"
							   << "	}"
							   << "});"
							   << "dispatchEvent(e);";
						   bwInfo->browser->GetMainFrame()->ExecuteJavaScript(js_event.str(), "", 0);
						   break;
	}// End case-WM_ACTIVATEAPP

	}// End switch-message
	return CallWindowProc((WNDPROC)bwInfo->oldMainProc, hWnd, message, wParam, lParam);
}
// ������������ڵ���Ϣ
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	// ���ԶԱ�Ҫ����Ϣ����Ԥ�ȴ�����Ҫ���ص���Ϣ�Ϳ��Բ��÷����������stringstream s;
	switch (message) {
	case WM_PARENTNOTIFY:
	{
#ifdef JWebTopLog
							stringstream sss;
							sss << "mid WM_PARENTNOTIFY[wParam=" << wParam << ",lParam=" << lParam << "]" << "\r\n";
							writeLog(sss.str());
#endif
							UINT msg2 = LOWORD(wParam);
							// ����ж���Ļ����Կ�����switch��ʽ
							if (msg2 == WM_LBUTTONDOWN){
								if (bwInfo->configs.enableDrag){// �Ƿ������϶�
									POINT pt;
									GetCursorPos(&pt);
									bwInfo->isDraging = true;
									bwInfo->dragX = LOWORD(lParam);
									bwInfo->dragY = HIWORD(lParam);
								}
							}
							else if (msg2 == WM_DESTROY){
								SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)bwInfo->oldBrowserProc);// ���û�ԭ���Ĵ�����
								BrowserWindowInfos.erase(hWnd);// �������map�е�����
							}
	}
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		bwInfo->isDraging = false;
		break;
	case WM_MOUSEMOVE:{
						  if (bwInfo->isDraging){
							  POINT pt;
							  GetCursorPos(&pt);
							  LONG x = pt.x - bwInfo->dragX;
							  LONG y = pt.y - bwInfo->dragY;
							  SetWindowPos(bwInfo->hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);// �ƶ����ڣ������ı䴰�ڴ�С�ʹ������ڲ��
						  }
	}
		break;
	case WM_KEYUP:
		if (wParam == VK_F1){// ����F12ʱ�򿪵��Թ���(F12��ʱ�ᴥ���쳣����ʱ��ΪF1)
			if (bwInfo->configs.enableDebug){
				CefPoint(pp);
				pp.x = 300;
				pp.y = 300;
				CefWindowInfo windowInfo;
				CefBrowserSettings settings;
				windowInfo.SetAsPopup(NULL, "cef_debug");
				CefRefPtr<CefBrowserHost> host = bwInfo->browser->GetHost();
				host->ShowDevTools(windowInfo, new DEBUG_Handler(), settings, pp);
			}
		}
		else if (wParam == VK_F11){// ����F12ʱ�򿪵��Թ���
			createNewBrowser(NULL);
		}
		break;
	}// End switch-message
	return CallWindowProc((WNDPROC)bwInfo->oldBrowserProc, hWnd, message, wParam, lParam);
}

// ����������Ϣ(configs)�Զ��㴰�ں�ʵ����������ڽ�������
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs * p_configs){
	JWebTopConfigs configs = (* p_configs);
	HWND hWnd = browser->GetHost()->GetWindowHandle();// ��������ڴ��ڵ�handle
	WINDOWINFO winInfo;
	GetWindowInfo(hWnd, &winInfo);// ��ȡ������Ϣ
	bool changed = false;
	//����������Ϣ�Դ������½���װ��(url��title��createBrowerʱ�����ˣ�
	if (configs.dwStyle != 0){
		DWORD dwStyle = winInfo.dwStyle;
		dwStyle &= configs.dwStyle;//��λ�뽫����ʽȥ��  
		SetWindowLong(hWnd, GWL_STYLE, dwStyle);//���ó��µ���ʽ
		changed = true;
	}
	if (configs.dwExStyle != 0){
		DWORD  dwExStyle = winInfo.dwExStyle;
		dwExStyle &= configs.dwExStyle;//��λ�뽫����չ��ʽȥ��  
		SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle);//�����µ���չ��ʽ  
		changed = true;
	}
	if (!configs.icon.empty()){
		HICON hIcon = GetIcon(configs.url, configs.icon);
		SetClassLong(hWnd, GCL_HICON, (LONG)hIcon);
	}
	if (configs.x != -1 || configs.y != -1 || configs.w != -1 || configs.h != -1){
		RECT rc = winInfo.rcWindow;
		SetWindowPos(hWnd, HWND_TOPMOST,
			configs.x == -1 ? rc.left : configs.x, configs.y == -1 ? rc.top : configs.y,
			configs.w == -1 ? rc.right : configs.w, configs.h == -1 ? rc.bottom : configs.h,
			SWP_ASYNCWINDOWPOS | SWP_FRAMECHANGED | SWP_NOCOPYBITS | SWP_NOZORDER);
		changed = true;
	}
	HWND bWnd = GetNextWindow(hWnd, GW_CHILD);// �õ���ʵ�����������
	//if (changed){// ������ڵ�״̬�����˸ı�
	//	//RECT rc;
	//	//GetWindowRect(hWnd, &rc);// ��ȡ���ڴ�С����������б߿�ͱ������������ϵģ�
	//	//MoveWindow(bWnd, 0, 0, rc.right - rc.left, rc.bottom - rc.top, true);// �����ƶ�֮���п��ܻ�����������ʵ�ʴ�С�����ԡ����ڴ��ڿ����������Ӱ�첻��
	//}
	LONG preWndProc = GetWindowLongPtr(bWnd, GWLP_WNDPROC);
	if (preWndProc != (LONG)JWebTop_BrowerWndProc){
		SetWindowLongPtr(bWnd, GWLP_WNDPROC, (LONG)JWebTop_BrowerWndProc);
		BrowserWindowInfo * bwInfo = new BrowserWindowInfo();
		bwInfo->hWnd = hWnd;
		bwInfo->bWnd = bWnd;
		bwInfo->oldBrowserProc = preWndProc;
		preWndProc = GetWindowLongPtr(hWnd, GWLP_WNDPROC);
		bwInfo->oldMainProc = preWndProc;
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)JWebTop_WindowWndProc);// �滻�����ڵ���Ϣ������
		bwInfo->browser = browser;
		bwInfo->configs = configs;
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(bWnd, bwInfo));// ��map�����м�¼��hWnd��֮ǰWndProc�Ĺ�ϵ
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(hWnd, bwInfo));// ��map�����м�¼��hWnd��֮ǰWndProc�Ĺ�ϵ
	}
#ifdef JWebTopLog
	stringstream ss;
	ss << "hWnd===" << hWnd << ",bWnd=" << bWnd << ",rWnd=<<" << GetNextWindow(bWnd, GW_CHILD) //
		<< ",pWnd=" << ::GetAncestor(browser->GetHost()->GetWindowHandle(), GA_ROOT)
		<< ",aWnd1=" << ::GetAncestor(hWnd, GA_ROOT)
		<< ",aWnd2=" << ::GetAncestor(bWnd, GA_ROOT)
		<< "\r\n";
	writeLog(ss.str());
#endif 
}

namespace jw{
	// getPos(handler);//��ô���λ�ã�����ֵΪһobject����ʽ����{x:13,y:54}
	POINT getPos(HWND hWnd){
		RECT rt = getBound(hWnd);
		POINT p;
		p.x = rt.left;
		p.y = rt.top;
		return p;
	}
	// setSize(x, y, handler);//���ô��ڴ�С
	void setSize(HWND hWnd, int w, int h){
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);// ֻ�ı䴰�ڴ�С�����ı䴰������ʹ������ڲ��
	}
	// getSize(handler);//��ô��ڴ�С������ֵΪһobject����ʽ����{width:130,height:54}
	POINT getSize(HWND hWnd){
		RECT rt = getBound(hWnd);
		POINT p;
		p.x = rt.right;
		p.y = rt.bottom;
		return p;
	}
	// getScreenSize();//��ȡ��Ļ��С������ֵΪһobject����ʽ����{width:130,height:54}
	POINT getScreenSize(){
		POINT p;
		p.x = GetSystemMetrics(SM_CXFULLSCREEN);
		p.y = GetSystemMetrics(SM_CYFULLSCREEN) + GetSystemMetrics(SM_CYCAPTION);
		return p;
	}
	// move(x, y, handler);//�ƶ�����
	void move(HWND hWnd, int x, int y){
		::SetWindowPos(hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOSIZE | SWP_NOZORDER | SWP_NOSENDCHANGING);// �ƶ����ڣ������ı䴰�ڴ�С�ʹ������ڲ��
	}
	// setBound(x, y,w ,h, handler);// ͬʱ���ô��ڵ�����ʹ�С
	void setBound(HWND hWnd, int x, int y, int w, int h){
		::MoveWindow(hWnd, x, y, w, h, false);
	}
	// ��ȡ���ڵ�λ�úʹ�С������ֵΪһobject����ʽ����{x:100,y:100,width:130,height:54}
	RECT getBound(HWND hWnd){
		RECT rt;
		GetWindowRect(hWnd, &rt);
		return rt;
	}
	// setTitle(title, handler);// ���ô�������
	void setTitle(HWND hWnd, wstring title){
		SetWindowText(hWnd, title.c_str());
	}
	// getTitle(handler);// ��ȡ�������ƣ�����ֵΪһ�ַ���
	wstring getTitle(HWND hWnd){
		TCHAR title[1000];// ��������ʼ��
		::GetWindowText(hWnd, title, sizeof(title));// ��ȡ��������
		return title;
	}
	// bringToTop(handler);//�����Ƶ����
	void bringToTop(HWND hWnd){
		SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	}
	// focus(handler);//ʹ���ڻ�ý���
	void focus(HWND hWnd){
		SetFocus(hWnd);
	}
	// hide(handler);//���ش���
	void hide(HWND hWnd){
		ShowWindow(hWnd, SW_HIDE);
	}
	// max(handler);//��󻯴���
	void max(HWND hWnd){
		setBound(hWnd, 0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN) + GetSystemMetrics(SM_CYCAPTION));
	}
	// mini(hander);//��С������
	void mini(HWND hWnd){
		if (GetWindowLong(hWnd, GWL_EXSTYLE)&WS_EX_TOOLWINDOW){
			ShowWindow(hWnd, SW_HIDE);
		}
		else{
			ShowWindow(hWnd, SW_MINIMIZE);
		}
	}
	// restore(handler);//��ԭ���ڣ���Ӧ��hide����
	void restore(HWND hWnd){
		ShowWindow(hWnd, SW_RESTORE);
	}
	// setTopMost(handler);//�����ö����˺�����bringToTop���������ڴ˺�����ʹ������Զ�ö�������������һ�����ڵ������ö�����
	void setTopMost(HWND hWnd){
		DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);//��ȡ����ʽ  
		DWORD dwNewExStyle = WS_EX_TOPMOST;
		dwNewExStyle |= dwExStyle;//��λ�뽫����չ��ʽȥ��  
		SetWindowLong(hWnd, GWL_EXSTYLE, dwNewExStyle);//�����µ���չ��ʽ
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	//close(handler);// �رմ���
	void close(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL)bw->browser->GetHost()->CloseBrowser(true);
	}

	//setWindowStyle(exStyle, handler);//�߼����������ô��ڶ������ԣ������ö�֮�ࡣ
	void setWindowStyle(HWND hWnd, int exStyle){
		HWND hWndTemp = hWnd;
		DWORD dwExStyle = GetWindowLong(hWndTemp, GWL_EXSTYLE);//��ȡ����ʽ  
		exStyle |= dwExStyle;//��λ�뽫����չ��ʽȥ��  
		SetWindowLong(hWndTemp, GWL_EXSTYLE, exStyle);//�����µ���չ��ʽ  
	}
	//loadUrl(url, handler);//������ҳ��urlΪ��ҳ·��
	void loadUrl(HWND hWnd, std::wstring url){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL)bw->browser->GetMainFrame()->LoadURL(url);
	}
	//reload(handler);//���¼��ص�ǰҳ��
	void reload(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL)bw->browser->Reload();
	}
	//reloadIgnoreCache(handler);//���¼��ص�ǰҳ�沢���Ի���
	void reloadIgnoreCache(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL)bw->browser->ReloadIgnoreCache();
	}
	//showDev(handler);//�򿪿����߹���
	void showDev(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL){
			CefWindowInfo windowInfo;
			windowInfo.SetAsPopup(NULL, "cef_debug");
			bw->browser->GetHost()->ShowDevTools(windowInfo, new DEBUG_Handler(), CefBrowserSettings(), CefPoint());
		}
	}

	void ExecJS(HWND hWnd, string js){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL){
			bw->browser->GetMainFrame()->ExecuteJavaScript(CefString(js), "", 0);
		}
	}
	void ExecJS(HWND hWnd, wstring js){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL){
			bw->browser->GetMainFrame()->ExecuteJavaScript(CefString(js), "", 0);
		}
	}
}