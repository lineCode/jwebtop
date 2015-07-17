#include "JWebTopWinCtrl.h"

#include <sstream>
#include <string>
#include "JWebTop/browser/JWebTopCommons.h"
#include "JWebTop/util/StrUtil.h"
#include "JWebTop/tests/TestUtil.h"
using namespace std;
BrowerWindowInfoMap BrowerWindowInfos;// �ھ�̬�����л��������Ѵ����Ĵ�����Ϣ

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

BrowerWindowInfo getBrowserWindowInfo(HWND hWnd){
	BrowerWindowInfo bwInfo;
	BrowerWindowInfoMap::iterator it = BrowerWindowInfos.find(hWnd);
	if (BrowerWindowInfos.end() != it) {
		bwInfo = it->second;
	}
	return bwInfo;
}
// ���ڵ��Թ��ߴ��ڵ�cefclient���������JWebTopHandler��Ѷ�JWebTop�Ĵ���������DEBUG��ҲӦ��һ�Σ�
class DEBUG_Handler : public CefClient{ IMPLEMENT_REFCOUNTING(DEBUG_Handler); };
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowerWindowInfo bwInfo = getBrowserWindowInfo(hWnd);
	// ���ԶԱ�Ҫ����Ϣ����Ԥ�ȴ�����Ҫ���ص���Ϣ�Ϳ��Բ��÷����������
	switch (message) {
	case WM_CLOSE:
		BrowerWindowInfos.erase(hWnd);// �����ڹر�ʱ����������
		break;
	case WM_LBUTTONDOWN:// ������
		bwInfo.isDraging = true;
		bwInfo.dragX = LOWORD(lParam);
		bwInfo.dragY = HIWORD(lParam);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		bwInfo.isDraging = false;
		break;
	case WM_MOUSEMOVE:{
						  POINT pt;
						  GetCursorPos(&pt);
						  RECT rc,hrc;
						  GetWindowRect(hWnd, &rc);
						  GetWindowRect(bwInfo.hWnd, &hrc);
						  int x = pt.x - bwInfo.dragX+hrc.left;
						  int y = pt.y - bwInfo.dragY+hrc.top;
#ifdef WebTopLog
						  stringstream s;
						  s << "   mv[x=" << x << ",y=" << y << "] cased_mid:" << message << "\r\n";
						  writeLog(s.str());
#endif
						  // �������ַ�ʽ�������϶�
						  //MoveWindow(bwInfo.hWnd, x, y, rc.right, rc.bottom, true);
						  SetWindowPos(bwInfo.hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
						  break;
	}
	case WM_KEYUP:
		if (wParam == VK_F12){// ����F12ʱ�򿪵��Թ���(��ʱ�ᴥ���쳣)
			if (bwInfo.configs.enableDebug){
				CefPoint(pp);
				pp.x = 300;
				pp.y = 300;
				CefWindowInfo windowInfo;
				CefBrowserSettings settings;
				windowInfo.SetAsPopup(NULL, "cef_debug");
				CefRefPtr<CefBrowserHost> host = bwInfo.browser->GetHost();
				host->ShowDevTools(windowInfo, new DEBUG_Handler(), settings, pp);
			}
		}
		else if (wParam == VK_F11){// ����F12ʱ�򿪵��Թ���
			createNewBrowser(NULL);
		}
		break;
	case WM_PARENTNOTIFY:
		break;
	}
	return CallWindowProc((WNDPROC)getBrowserWindowInfo(hWnd).oldProc, hWnd, message, wParam, lParam);
}

// ����������Ϣ(configs)�Զ��㴰�ں�ʵ����������ڽ�������
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs configs){
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
	if (changed){// ������ڵ�״̬�����˸ı�
		//RECT rc;
		//GetWindowRect(hWnd, &rc);// ��ȡ���ڴ�С����������б߿�ͱ������������ϵģ�
		//MoveWindow(bWnd, 0, 0, rc.right - rc.left, rc.bottom - rc.top, true);// �����ƶ�֮���п��ܻ�����������ʵ�ʴ�С�����ԡ����ڴ��ڿ����������Ӱ�첻��
	}
	LONG preWndProc = GetWindowLongPtr(bWnd, GWLP_WNDPROC);
	if (preWndProc != (LONG)JWebTop_BrowerWndProc){
		SetWindowLongPtr(bWnd, GWLP_WNDPROC, (LONG)JWebTop_BrowerWndProc);
		//SetWindowLongPtr(hWnd, GWLP_USERDATA, preWndProc);// ����CEF��ʹ��GWLP_USERDATA�������������ò����ã������ᵼ�³���
		BrowerWindowInfo bwInfo;
		bwInfo.hWnd = hWnd;
		bwInfo.bWnd = bWnd;
		bwInfo.oldProc = preWndProc;
		bwInfo.browser = browser;
		bwInfo.configs = configs;
		BrowerWindowInfos.insert(std::pair<HWND, BrowerWindowInfo>(bWnd, bwInfo));// ��map�����м�¼��hWnd��֮ǰWndProc�Ĺ�ϵ
	}
}