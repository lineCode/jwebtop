#include "JWebTopCommons.h"
#include "JWebTopClient.h"
#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"
#include <string>
using namespace std;


CefRefPtr<JWebTopClient> g_handler;// ȫ�ֱ���һ��JWebTopClient����
// ��ʱ��¼����������Ϣ��������JWebTopBrowser��JWebTopClient���ݲ���������ΪJWebTopClient��ȫ��Ψһʵ����ʹ�ú��ÿ�
JWebTopConfigs  tmpConfigs;

extern LPTSTR g_lpCmdLine;// 
// �õ���������Ŀ¼
wstring GetExePath(){
	TCHAR   szPath[1000];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	wstring path(szPath);
	path = path.substr(0, path.find_last_of('\\') + 1);
	return path;
}

void createNewBrowser(LPCTSTR appDefFile){
	wstring pp;
	if (appDefFile == NULL){// δ���������ļ�
		if (g_lpCmdLine == NULL || lstrlen(g_lpCmdLine)==0){// δָ�������ļ���������Ƿ�ͨ����������ָ����appDefFile
			pp += GetExePath() + L"index.app";// ȡ��������Ŀ¼�µ�index.app�ļ�
		}
		else{
			pp = g_lpCmdLine;// ȡ����������ΪappDefFile·��
		}
	}
	createNewBrowser(loadConfigs(pp));
}

void createNewBrowser(JWebTopConfigs configs){
	CEF_REQUIRE_UI_THREAD();
	// ���ڹ������ش��ڵ�������Ϣ��ע����Щ��Ϣ[style��exStyle]�����˲�̫���ã�������Handler��OnAfterCreated�жԴ��ڽ������Σ�
	CefWindowInfo window_info;

#if defined(OS_WIN)
	// On Windows we need to specify certain flags that will be passed to
	window_info.SetAsPopup(NULL, configs.name);
#endif
	if (g_handler == NULL)g_handler = new JWebTopClient();
	CefBrowserSettings browser_settings;
	//window_info.window_name = UUID::Data1();// ��UUID��Ϊ�������ƣ�������������JWebTopConfigs�Ĺ���������
	tmpConfigs = configs;
	// �������������
	CefBrowserHost::CreateBrowser(window_info, g_handler.get(), configs.getAbsolutePath(configs.url.ToWString()), browser_settings, NULL);
}
