// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "JWebTopClient.h"

#include <sstream>
#include <string>
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "JWebTop/wndproc/JWebTopWndProc.h"
#include "JWebTop/dllex/JWebTop_DLLEx.h"
#include "common/util/StrUtil.h"
#include "common/os/OS.h"
#include "JWebTop/jshandler/JJH_Windows.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
#include "JWebTopContext.h"
#include "JWebTopCommons.h"

using namespace std;
namespace jw{
	extern JWebTopConfigs * g_configs;  // Ӧ������ʱ�ĵ�һ�����ñ���
}
JWebTopClient::JWebTopClient()
: dialog_handler_(new JSDialogHandler()){
}

JWebTopClient::~JWebTopClient() {}

extern CefSettings settings;              // CEFȫ������

void JWebTopClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	jw::ctx::addBrowser(browser);// ��¼���Ѿ������Ĵ�����
	if (jw::dllex::ex()) {
		jw::dllex::sendBrowserCreatedMessage(this->taskId, (LONG)browser->GetHost()->GetWindowHandle());
	}
	renderBrowserWindow(browser, this->configs);
	if (!message_router_) {
		CefMessageRouterConfig config;
		message_router_ = CefMessageRouterBrowserSide::Create(config);
		// Register handlers with the router.
		jc::CreateBrowserSideMessageHandlers(message_handler_set_);
		MessageHandlerSet::const_iterator it = message_handler_set_.begin();
		for (; it != message_handler_set_.end(); ++it)
			message_router_->AddHandler(*(it), false);
	}
}

bool JWebTopClient::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	bool is_redirect) {
	CEF_REQUIRE_UI_THREAD();
	message_router_->OnBeforeBrowse(browser, frame);
	return false;
}

bool JWebTopClient::OnProcessMessageReceived(
	CefRefPtr<CefBrowser> browser,
	CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message) {
	CEF_REQUIRE_UI_THREAD();
	if (message_router_->OnProcessMessageReceived(browser, source_process,
		message)) {
		return true;
	}
	//// Check for messages from the client renderer.
	//std::string message_name = message->GetName();
	//if (message_name == kFocusedNodeChangedMessage) {
	//	// A message is sent from ClientRenderDelegate to tell us whether the
	//	// currently focused DOM node is editable. Use of |focus_on_editable_field_|
	//	// is redundant with CefKeyEvent.focus_on_editable_field in OnPreKeyEvent
	//	// but is useful for demonstration purposes.
	//	focus_on_editable_field_ = message->GetArgumentList()->GetBool(0);
	//	return true;
	//}
	return false;
}
void JWebTopClient::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
	TerminationStatus status) {
	CEF_REQUIRE_UI_THREAD();
	message_router_->OnRenderProcessTerminated(browser);
}

bool JWebTopClient::GetAuthCredentials(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	bool isProxy,
	const CefString& host,
	int port,
	const CefString& realm,
	const CefString& scheme,
	CefRefPtr<CefAuthCallback> callback) {
	CEF_REQUIRE_IO_THREAD();
	if (isProxy) {// ���ڴ��������Ҫ��֤���ṩ�û���������
		//if(frame->GetURL...)// ע������û�������ĸ������ĸ�url
		callback->Continue(jw::g_configs->proxyAuthUser, jw::g_configs->proxyAuthPwd);
		return true;
	}
	return false;
}
bool JWebTopClient::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();
	HWND hWnd = browser->GetHost()->GetWindowHandle();
	if (hWnd != NULL || getBrowserWindowInfo(hWnd) == NULL){
		OnBeforeClose(browser);
	}
	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void JWebTopClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();
	jw::ctx::removeBrowser(browser);
	if (jw::dllex::ex()) {
		jw::dllex::invokeRemote_NoWait(browser->GetHost()->GetWindowHandle(),"{\"method\":\"browserClosed\"}");
	}
}
void JWebTopClient::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl) {
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED) return;
	frame->LoadString(L"<html><body><h2>�����ˣ���ַ��" + failedUrl.ToWString()+ L"����</h2></body></html>", failedUrl);
}

// �жϸ��ӵ�js�Ƿ���Ҫ�����÷�ʽ���
bool isReference(const wstring &appendFile){
	wstring start = appendFile.substr(0, 10);
	int len = start.length();
	if (len < 7/*7=="http://".length()*/ || start.find(L":") == -1)return 0;
	//for (int i = 0; i < len; i++){
	//	start[i] = tolower(start[i]);
	//}
	transform(start.begin(), start.end(), start.begin(), ::tolower);
	return start.find(L"http://") != -1 || start.find(L"https://") != -1 || start.find(L"jwebtop://") != -1;
}

void JWebTopClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode){
	CefRefPtr<CefBrowserHost> host = browser->GetHost();
	HWND hWnd = host->GetWindowHandle();
	// ���JWebTop�����handler���Ժ�close�������ŵ�OnAfterCreated�У�ҳ�����¼��غ����ͱ����ᶪʧ��
	stringstream extensionCode;
	extensionCode << "if(!JWebTop)JWebTop={};";
	extensionCode << "(function() {";// ��OnLoad��JWebTop��̬��ӵ�JS�ű�������������������û�JS�ļ��ĳ�ͻ
	extensionCode << "JWebTop.handler=" << (LONG)hWnd << ";" << endl;
	extensionCode << "JWebTop.cefQuery = function(ajson){ window.cefQuery({ request:JSON.stringify(ajson) }) }; " << endl;// ��װ��window.cefQuery����
	if (!settings.single_process){	// �����ģʽ�£���Ҫ��������Ϣ�ķ�ʽע����Ҫ����HWND��ȡBorwser�ĺ�����JWebTop����
		// close(handler);// �رմ���
		extensionCode << "JWebTop.close=function(handler){JWebTop.cefQuery({m:'close',handler:(handler?handler:JWebTop.handler)})};" << endl;
		//loadUrl(url, handler);//������ҳ��urlΪ��ҳ·��
		extensionCode << "JWebTop.loadUrl=function(url,handler){JWebTop.cefQuery({m:'loadUrl',url:url,handler:(handler?handler:JWebTop.handler)})};" << endl;
		//reload(handler);//���¼��ص�ǰҳ��
		extensionCode << "JWebTop.reload=function(handler){JWebTop.cefQuery({m:'reload',handler:(handler?handler:JWebTop.handler)})};" << endl;
		//reloadIgnoreCache(handler);//���¼��ص�ǰҳ�沢���Ի���
		extensionCode << "JWebTop.reloadIgnoreCache=function(handler){JWebTop.cefQuery({m:'reloadIgnoreCache',handler:(handler?handler:JWebTop.handler)})};" << endl;
		//showDev(handler);//�򿪿����߹���
		extensionCode << "JWebTop.showDev=function(handler){JWebTop.cefQuery({m:'showDev',handler:(handler?handler:JWebTop.handler)})};" << endl;

		// enableDrag(true|false);// ��������϶�
		extensionCode << "JWebTop.enableDrag=function(enable){JWebTop.cefQuery({m:'enableDrag',enable:enable,handler:JWebTop.handler})};" << endl;
		// startDrag();// ��ʼ�����϶�
		extensionCode << "JWebTop.startDrag=function(){JWebTop.cefQuery({m:'startDrag',handler:JWebTop.handler})};" << endl;
		// stopDrag();// ֹͣ�϶�
		extensionCode << "JWebTop.stopDrag=function(){JWebTop.cefQuery({m:'stopDrag',handler:JWebTop.handler})};" << endl;
	}
	// runApp(appName,handler);//����һ��app��appNameΪ.app�ļ�·����
	extensionCode << "JWebTop.runApp=function(app,parentWin,handler){JWebTop.cefQuery({m:'runApp',app:app,parentWin:(parentWin?parentWin:0),handler:(handler?handler:JWebTop.handler)})};" << endl;
	extensionCode << "JWebTop.runAppMore=function(app,parentWin,url,name,icon,x,y,w,h,handler){" \
		"var morejson={m:'runAppMore',app:app,parentWin:(parentWin?parentWin:0),handler:(handler?handler:JWebTop.handler)};" \
		"if(url!=null)morejson.url=url;" \
		"if(name!=null)morejson.name=name;" \
		"if(icon!=null)morejson.icon=icon;" \
		"if(x!=null)morejson.x=x;" \
		"if(y!=null)morejson.y=y;" \
		"if(w!=null)morejson.w=w;" \
		"if(h!=null)morejson.h=h;" \
		"JWebTop.cefQuery(morejson)};";
	if (!configs->appendJs.empty()){// ��Ҫ����һ��js�ļ�
		wstring appendFile = configs->appendJs.ToWString();
		//�������ַ�ʽ���п������⣬���Բ��ö����ļ��ķ�ʽ
		if (isReference(ref(appendFile))){// ͨ��docuemnt.wirte�ķ�ʽ��д
			appendFile = jw::replace_allW(ref(appendFile), L"\\", L"/");// �滻�ļ��еĻ��з���	
			extensionCode
				<< "\r\n var scriptLet = document.createElement('SCRIPT');"
				<< "\r\n scriptLet.type = 'text/javascript';"
				<< "\r\n scriptLet.src = '" << jw::w2s(appendFile) << "';"
				<< "\r\n addEventListener(\"JWebTopReady\",function(){document.body.appendChild(scriptLet);});"
				<< "\r\n";
		}
		else{
			appendFile = configs->getAbsolutePath(appendFile);
			string appendJS;
			if (jw::readfile(appendFile, ref(appendJS))){
				extensionCode << "\r\n" << appendJS << "\r\n";
			}
		}
	}
	extensionCode << "})()\r\n";// �����Խű��İ�Χ
#ifdef JWebTopLog
	writeLog(extensionCode.str());
#endif
	browser->GetMainFrame()->ExecuteJavaScript(CefString(extensionCode.str()), "", 0);
	jw::js::events::sendReadey(browser->GetMainFrame());
	if (configs->enableResize)jb::checkAndSetResizeAblity(hWnd);
}