#include "JWebTopWndProc.h"

#include <thread>
#include "JWebTop/browser/JWebTopCommons.h"
#include "common/util/StrUtil.h"
#include "common/winctrl/JWebTopWinCtrl.h"
#include "JWebTop/dllex/JWebTop_DLLEx.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif

using namespace std;
BrowserWindowInfoMap BrowserWindowInfos;// �ھ�̬�����л��������Ѵ����Ĵ�����Ϣ

BrowserWindowInfo * getBrowserWindowInfo(HWND hWnd){
	BrowserWindowInfoMap::iterator it = BrowserWindowInfos.find(hWnd);
	if (BrowserWindowInfos.end() != it) {
		return it->second;
	}
	return NULL;
}

namespace jb{
	//close(handler);// �رմ���
	void close(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL)bw->browser->GetHost()->CloseBrowser(true);
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
	void CreateNewBrowserThread(JWebTopConfigs *tmpConfigs){
		createNewBrowser(tmpConfigs);
	}
	void runApp(HWND hWnd, wstring appDefFile, long parentWin, const LPTSTR url, const LPTSTR &title, const LPTSTR &icon, const  int x, const  int y, const  int w, const  int h){
#ifdef JWebTopLog 
		std::wstringstream log;
		log << L"run app=" << appDefFile << L",parentWin=" << parentWin << L"\r\n";
		writeLog(log.str());
#endif
		//if (tmpConfigs != g_configs)delete tmpConfigs;
		JWebTopConfigs *tmpConfigs = JWebTopConfigs::loadConfigs(JWebTopConfigs::getAppDefFile(appDefFile.c_str()));
		tmpConfigs->parentWin = parentWin;

		if (url != NULL)tmpConfigs->url = CefString(url);
		if (title != NULL)tmpConfigs->url = CefString(title);
		if (icon != NULL)tmpConfigs->url = CefString(icon);
		if (x != -1)tmpConfigs->x = x;
		if (y != -1)tmpConfigs->y = y;
		if (h != -1)tmpConfigs->h = h;
		if (w != -1)tmpConfigs->w = w;
		thread t(CreateNewBrowserThread, tmpConfigs);
		t.detach();
	}

	void enableDrag(HWND hWnd, bool enable){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw == NULL)return;
		bw->enableDrag = enable;
	}
	void startDrag(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw == NULL || !bw->enableDrag)return;
		POINT pt;
		RECT rt;
		GetWindowRect(hWnd, &rt);
		GetCursorPos(&pt);
		// ���㲢�����������
		bw->dragX = pt.x - rt.left;
		bw->dragY = pt.y - rt.top;
		bw->isDraging = true;// ���Ϊ��ʼ�϶�
	}
	void stopDrag(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw == NULL)return;
		bw->isDraging = false;// ֹͣ�϶�
	}
}

HICON GetIcon(CefString url, CefString path){
	if (path.ToWString().find(L":") == -1){// ���ָ����·�������·��
		wstring _path;
		_path = url.ToWString();
		jw::replace_allW(_path, L"\\", L"/");
		_path = _path.substr(0, _path.find_last_of('/') + 1);
		path = _path.append(path);
	}
	return (HICON)::LoadImage(NULL, path.ToWString().data(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
}

// ���������ڵ���Ϣ����Щ��Ϣ������رմ��ڡ��ƶ����ڵȣ�ֻ���������ڲ���������
LRESULT CALLBACK JWebTop_WindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	// ���ԶԱ�Ҫ����Ϣ����Ԥ�ȴ�����Ҫ���ص���Ϣ�Ϳ��Բ��÷����������stringstream s;
	switch (message) {
	case WM_COPYDATA:{
						 return	 jw::dllex::onBrowserWinWmCopyData(hWnd, message, wParam, lParam);

	}
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
							UINT msg2 = LOWORD(wParam);
							if (msg2 == WM_DESTROY){
								if (bwInfo == NULL){
									DefWindowProc(hWnd, message, wParam, lParam);
								}
								else{
									SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)bwInfo->oldBrowserProc);// ���û�ԭ���Ĵ�����
									BrowserWindowInfos.erase(hWnd);// �������map�е�����
								}
							}
	}
		break;
	case WM_KILLFOCUS:
		bwInfo->isDraging = false; // ʧȥ����ʱ��ֹͣ�϶�
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
	}// End switch-message
	return CallWindowProc((WNDPROC)bwInfo->oldBrowserProc, hWnd, message, wParam, lParam);
}

// ����������Ϣ(configs)�Զ��㴰�ں�ʵ����������ڽ�������
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs * p_configs){
	JWebTopConfigs configs = (*p_configs);
	HWND hWnd = browser->GetHost()->GetWindowHandle();// ��������ڴ��ڵ�handle
	WINDOWINFO winInfo;
	GetWindowInfo(hWnd, &winInfo);// ��ȡ������Ϣ
	////����������Ϣ�Դ������½���װ��(url��title��style��dwStyle��createBrowerʱ�����ˣ�
	if (!configs.icon.empty()){
		HICON hIcon = GetIcon(configs.url, configs.icon);
		SetClassLong(hWnd, GCL_HICON, (LONG)hIcon);
	}
	if (configs.max){// ��Ҫ����󻯵ķ�ʽ����ʾ
		jw::max(hWnd);
	}
	HWND bWnd = GetNextWindow(hWnd, GW_CHILD);// �õ���ʵ�����������
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
		bwInfo->enableDrag = configs.enableDrag;
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(bWnd, bwInfo));// ��map�����м�¼��hWnd��֮ǰWndProc�Ĺ�ϵ
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(hWnd, bwInfo));// ��map�����м�¼��hWnd��֮ǰWndProc�Ĺ�ϵ
	}
#ifdef JWebTopLog
	stringstream ss;
	ss << "hWnd===" << hWnd << ",bWnd=" << bWnd << ",rWnd=<<" << GetNextWindow(bWnd, GW_CHILD) //
		<< ",parentWin=" << configs.parentWin
		<< "\r\n";
	writeLog(ss.str());
#endif 
}