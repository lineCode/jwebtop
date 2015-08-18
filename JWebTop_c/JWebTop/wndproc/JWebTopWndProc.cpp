#include "JWebTopWndProc.h"

#include "JWebTop/browser/JWebTopCommons.h"

#include "common/JWebTopMsg.h"
#include "common/util/StrUtil.h"
#include "common/process/MultiProcess.h"
#include "common/winctrl/JWebTopWinCtrl.h"
#include "common/task/Task.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif

using namespace std;
BrowserWindowInfoMap BrowserWindowInfos;// �ھ�̬�����л��������Ѵ����Ĵ�����Ϣ

extern JWebTopConfigs * g_configs;
extern JWebTopConfigs * tmpConfigs;

BrowserWindowInfo * getBrowserWindowInfo(HWND hWnd){
	BrowserWindowInfoMap::iterator it = BrowserWindowInfos.find(hWnd);
	if (BrowserWindowInfos.end() != it) {
		return it->second;
	}
	return NULL;
}

extern HWND g_RemoteWinHWnd;
namespace jb{
	bool sendJWebTopProcessMsg(HWND browserHWnd, DWORD msgId, LPTSTR msg){
		BrowserWindowInfo * bw = getBrowserWindowInfo(browserHWnd);
		if (bw == NULL)return false;
		if (g_RemoteWinHWnd == NULL)return false;
		return jw::sendProcessMsg(g_RemoteWinHWnd, msgId, msg);
	}

	CefString invokeRemote_Wait(HWND browserHWnd, CefString json){
		wstring taskId = jw::task::createTaskId();			         // ��������id
		// taskId���ӵ�json�ַ�����
		wstring newjson = json.ToWString();
		wstring wrapped = jw::wrapAsTaskJSON((long)browserHWnd, std::ref(taskId), std::ref(newjson));
		jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // �������������
		if (sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_WAIT, LPTSTR(wrapped.c_str()))){ // ��������Զ�̽���
			lock->wait();		             		 		             // �ȴ��������
			wstring result = lock->result;   		 		             // ȡ��ִ�н��
			return CefString(result);									 // ��������
		}
		else{
			jw::task::removeTask(taskId);								// ��Ϣ����ʧ���Ƴ�������Ϣ
			return CefString();											// �������ݣ�ע�������ǿ��ַ���
		}
	}

	void invokeRemote_NoWait(HWND browserHWnd, CefString json){
		sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_RETURN, LPTSTR(json.ToWString().c_str())); // ��������Զ�̽���
	}
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
	void CreateNewBrowserThread(){
		createNewBrowser(tmpConfigs);
	}
	void runApp(HWND hWnd, std::wstring appDefFile, long parentWin){
#ifdef JWebTopLog 
		std::wstringstream log;
		log << L"run app=" << appDefFile << L",parentWin=" << parentWin << L"\r\n";
		writeLog(log.str());
#endif
		//if (tmpConfigs != g_configs)delete tmpConfigs;
		tmpConfigs = JWebTopConfigs::loadConfigs(JWebTopConfigs::getAppDefFile(appDefFile.c_str()));
		tmpConfigs->parentWin = parentWin;
		/*createNewBrowser(tmpConfigs);*/
		//DWORD  dwThreadId = 0; // ��¼�̵߳�id
		////LPTSTR str = L"abcd";
		//HANDLE threadHandle = // ��¼�̵߳�handler
		//CreateThread(NULL, 0, CreateNewBrowserThread, NULL, NULL, &dwThreadId);
		////Sleep(200);
		//CloseHandle(threadHandle);
		thread t(CreateNewBrowserThread);
		t.detach();
	}


	void browserwin_thread_executeWmCopyData(HWND browserHWnd, DWORD msgId, wstring msg){
		BrowserWindowInfo * bwInfo = getBrowserWindowInfo(browserHWnd);		
		switch (msgId)
		{
		case JWM_RESULT_RETURN:// Զ����������ɣ�����������ˣ���Ҫ֪ͨ�����̵ĵȴ��߳�ȥ��ȡ���
		{
								   wstring taskId, result;
								   long remoteHWnd;
								   jw::parseMessageJSON(msg, ref(remoteHWnd), ref(taskId), ref(result));  // ��������Ϣ�н���������id����������
								   jw::task::putTaskResult(taskId, result);	   					   // ֪ͨ�ȴ��̣߳�Զ����������ɣ������ȥȡ��
								   break;
		}
		case JWM_JSON_EXECUTE_WAIT:// Զ�̽��̷���һ�����񣬲���Զ�̽������ڵȴ���������ɺ���Ҫ����JWEBTOP_MSG_RESULT_RETURN��Ϣ��Զ�̽���
		case JWM_JS_EXECUTE_WAIT:// Զ�̽��̷���һ�����񣬲���Զ�̽������ڵȴ���������ɺ���Ҫ����JWEBTOP_MSG_RESULT_RETURN��Ϣ��Զ�̽���
		{

									 CefRefPtr<CefProcessMessage> cefMsg = CefProcessMessage::Create("waitjs");
									 CefRefPtr<CefListValue> args = cefMsg->GetArgumentList();
									 args->SetInt(0, msgId);
									 args->SetString(1, msg);
									 bwInfo->browser->SendProcessMessage(PID_RENDERER, cefMsg);// ֱ�ӷ��͵�render����ȥִ��
									 break;
		}
		case JWM_JSON_EXECUTE_RETURN:
			msg = L"invokeByDLL(" + msg + L")";// ��װjsonΪjs���� 
			bwInfo->browser->GetMainFrame()->ExecuteJavaScript(msg, "", 0);
			break;
		case JWM_JS_EXECUTE_RETURN:
			bwInfo->browser->GetMainFrame()->ExecuteJavaScript(msg, "", 0);
			break;
		default:
			break;
		}
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
LRESULT onBrowserWinWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	wstring msg;
	DWORD msgId = 0;
	jw::parseProcessMsg(lParam, ref(msgId), ref(msg));
	std::thread t(jb::browserwin_thread_executeWmCopyData, hWnd, msgId, msg);// onWmCopyData��ͬ����Ϣ��Ϊ�˷�ֹ��һ���̵ĵȴ������������߳��н���ҵ����
	t.detach();// �ӵ�ǰ�̷߳���
	return JWEBTOP_MSG_SUCCESS;
}
// ���������ڵ���Ϣ����Щ��Ϣ������رմ��ڡ��ƶ����ڵȣ�ֻ���������ڲ���������
LRESULT CALLBACK JWebTop_WindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	// ���ԶԱ�Ҫ����Ϣ����Ԥ�ȴ�����Ҫ���ص���Ϣ�Ϳ��Բ��÷����������stringstream s;
	switch (message) {
	case WM_COPYDATA:{
						 return	 onBrowserWinWmCopyData(hWnd, message, wParam, lParam);

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
#ifdef JWebTopLog
							stringstream sss;
							sss << "BrowerWndProc hWnd=" << hWnd << " WM_PARENTNOTIFY[wParam=" << wParam << ",lParam=" << lParam << "]" << "\r\n";
							writeLog(sss.str());
#endif
							UINT msg2 = LOWORD(wParam);
							// ����ж���Ļ����Կ�����switch��ʽ
							if (msg2 == WM_LBUTTONDOWN){
								if (bwInfo->enableDrag){// �Ƿ������϶�
									POINT pt;
									GetCursorPos(&pt);
									bwInfo->isDraging = true;
									bwInfo->dragX = LOWORD(lParam);
									bwInfo->dragY = HIWORD(lParam);
								}
							}
							else if (msg2 == WM_DESTROY){
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
	case WM_KEYUP:{
					  if (wParam == VK_F1){//
						  //if (bwInfo->configs.enableDebug){
						  CefPoint(pp);
						  pp.x = 300;
						  pp.y = 300;
						  CefWindowInfo windowInfo;
						  CefBrowserSettings settings;
						  windowInfo.SetAsPopup(NULL, "cef_debug");
						  CefRefPtr<CefBrowserHost> host = bwInfo->browser->GetHost();
						  host->ShowDevTools(windowInfo, new DEBUG_Handler(), settings, pp);
						  //}
					  }
					  else if (wParam == VK_F11){// 
						  createNewBrowser(NULL);
					  }
					  break;
	}break;
	}// End switch-message
	return CallWindowProc((WNDPROC)bwInfo->oldBrowserProc, hWnd, message, wParam, lParam);
}

// ����������Ϣ(configs)�Զ��㴰�ں�ʵ����������ڽ�������
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs * p_configs){
	JWebTopConfigs configs = (*p_configs);
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
		changed = true;
	}
	if (configs.max){// ��Ҫ����󻯵ķ�ʽ����ʾ
		jw::max(hWnd);
	}
	else if (configs.parentWin != 0){// û��ָ��������ʱ��������������ʾ����
		if (configs.x == -1 || configs.y == -1){
			RECT rc = winInfo.rcWindow;
			jw::setCenter(hWnd
				, configs.x == -1 ? rc.left : configs.x
				, configs.y == -1 ? rc.top : configs.y
				, rc.right - rc.left, rc.bottom - rc.top);
		}
	}
	else if (changed){// ������ڷ���иı䣬�ػ��´���
		RECT rc = winInfo.rcWindow;
		SetWindowPos(hWnd, HWND_TOPMOST, rc.left, rc.top, rc.right, rc.bottom, SWP_ASYNCWINDOWPOS | SWP_FRAMECHANGED | SWP_NOCOPYBITS | SWP_NOZORDER);
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
		if (configs.msgWin != 0) {
			std::wstringstream wss;
			wss << L"@{\"action\":\"browser\",\"method\":\"setBrowserHwnd\",\"msg\":\"������Ѵ���\",\"value\":{\"hwnd\":" << (LONG)hWnd << L"}}";
			jb::sendJWebTopProcessMsg(hWnd, 0, LPTSTR(wss.str().c_str()));
		}
	}
#ifdef JWebTopLog
	stringstream ss;
	ss << "hWnd===" << hWnd << ",bWnd=" << bWnd << ",rWnd=<<" << GetNextWindow(bWnd, GW_CHILD) //
		<< ",parentWin=" << configs.parentWin
		<< ",msgWin=" << configs.msgWin
		<< "\r\n";
	writeLog(ss.str());
#endif 
}