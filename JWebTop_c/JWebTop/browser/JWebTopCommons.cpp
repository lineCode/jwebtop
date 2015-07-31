#include "JWebTopCommons.h"
#include "JWebTopClient.h"
#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"
#include <string>
#include "JWebTop/winctrl/JWebTopWinCtrl.h"
#include "JWebTop/util/StrUtil.h"
#include "include/cef_parser.h"
#ifdef JWebTopLog
#include "JWebTop/tests/TestUtil.h"
#endif
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
			cef_json_parser_options_t options = cef_json_parser_options_t();// ����ʱ������
			CefRefPtr<CefValue> v= CefParseJSON(request, options);          // ���н���
			if (v == NULL){
				callback->Failure(1, CefString(L"�����JSON��ʽ"));
				return false;
			}
#ifdef JWebTopLog
			wstringstream log;
			log << L"JWebTopCommons#Handler#OnQuery jsons=" << request.ToWString() << L"\r\n";
			writeLog(log.str());
#endif
			CefRefPtr<CefDictionaryValue> value = v->GetDictionary();// ��JSON�ֵ�����ȡ
			wstring methodName = value->GetString("m").ToWString();  // ȡ��ĳ�ַ��� 
			if (methodName == L"close"){
				CefRefPtr<CefValue> handler = value->GetValue("handler");
				if (handler == NULL)return false;
				jw::close((HWND)handler->GetInt());
			}
			else if (methodName == L"loadUrl"){
				CefRefPtr<CefValue> handler = value->GetValue("handler");
				if (handler == NULL)return false;
				CefRefPtr<CefValue> url = value->GetValue("url");
				if (url == NULL)return false;
				jw::loadUrl((HWND)handler->GetInt(), url->GetString().ToWString());
			}
			else if (methodName == L"reload"){
				CefRefPtr<CefValue> handler = value->GetValue("handler");
				if (handler == NULL)return false;
				jw::reload((HWND)handler->GetInt());
			}
			else if (methodName == L"reloadIgnoreCache"){
				CefRefPtr<CefValue> handler = value->GetValue("handler");
				if (handler == NULL)return false;
				jw::reloadIgnoreCache((HWND)handler->GetInt());
			}
			else if (methodName == L"runApp"){
				CefRefPtr<CefValue> app = value->GetValue("app");
				CefRefPtr<CefValue> parentWin = value->GetValue("parentWin");
				jw::runApp(app->GetString().ToWString(), parentWin == NULL ? 0 : parentWin->GetInt());
			}
			else if (methodName == L"showDev"){
				CefRefPtr<CefValue> handler = value->GetValue("handler");
				if (handler == NULL)return false; 
				jw::showDev((HWND)handler->GetInt());
			}			
			return true;
		}
	};
	void CreateMessageHandlers(MessageHandlerSet& handlers) {
		handlers.insert(new Handler());
	}
}

CefRefPtr<JWebTopClient> g_handler;// ȫ�ֱ���һ��JWebTopClient����

extern JWebTopConfigs * tmpConfigs; // �����������ڶ���������й���ı���
extern JWebTopConfigs * g_configs;  // Ӧ������ʱ�ĵ�һ�����ñ���

void createNewBrowser(JWebTopConfigs * configs){
	CEF_REQUIRE_UI_THREAD();
	if (configs == NULL){
		tmpConfigs = g_configs;
	}
	else if (tmpConfigs != configs){
		if (configs != g_configs) delete tmpConfigs;
		tmpConfigs = configs;
	}
	// ���ڹ������ش��ڵ�������Ϣ��ע����Щ��Ϣ[style��exStyle]�����˲�̫���ã�������Handler��OnAfterCreated�жԴ��ڽ������Σ�
	CefWindowInfo window_info;

#ifdef JWebTopLog
	wstringstream ss;
	ss << L"JWebTopCommons createNewBrowser parentWin=" << tmpConfigs->parentWin << L"\r\n";
	writeLog(ss.str());
#endif
#if defined(OS_WIN)
	// On Windows we need to specify certain flags that will be passed to
	window_info.SetAsPopup((HWND)tmpConfigs->parentWin, tmpConfigs->name);
#endif
	if (g_handler == NULL)g_handler = new JWebTopClient();
	CefBrowserSettings browser_settings;
	//window_info.window_name = UUID::Data1();// ��UUID��Ϊ�������ƣ�������������JWebTopConfigs�Ĺ���������
	// �������������
	CefBrowserHost::CreateBrowser(window_info, g_handler.get(), tmpConfigs->getAbsolutePath(tmpConfigs->url.ToWString()), browser_settings, NULL);
}