#include "JWebTopCommons.h"
#include "JWebTopClient.h"
#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"
#include <string>
#include "JWebTop/winctrl/JWebTopWinCtrl.h"
using namespace std;

const char kTestMessageName[] = "close";
namespace jc/*jc=JWebTop Client*/{
	class Handler : public CefMessageRouterBrowserSide::Handler {
	public:
		Handler() {}

		// ��Ӧrender���̷��͹�������Ϣ
		virtual bool OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id,
			const CefString& request,// requestΪ��ϢJSON����{request:msg}��msg���֣�������չһ��JSON���������н��������������룺cef_message_router.cc=��CefMessageRouterRendererSideImpl��
			bool persistent, CefRefPtr<Callback> callback) OVERRIDE{
			//// �����Զ��ŷָ�����������
			//const std::string& message_name = request;
			//std::vector<int> vec;
			//const std::string& vals = message_name.substr(sizeof(kTestMessageName));std::stringstream ss(vals);int i;
			//while (ss >> i) {vec.push_back(i);if (ss.peek() == ',')ss.ignore();}
			//jw::close((HWND)vec[0]);// ���ַ�ʽ����ȡ����BrowerWindowInfoҲ��NULL������֣�
			if (request == "close"){
				callback.get()->Success(L"Close Ok!");
				browser->GetHost()->CloseBrowser(true);// �ر������
			}
			return true;
		}
	};
	void CreateMessageHandlers(MessageHandlerSet& handlers) {
		handlers.insert(new Handler());
	}
}

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