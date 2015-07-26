// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "JWebTopClient.h"

#include <sstream>
#include <string>
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "JWebTop/winctrl/JWebTopWinCtrl.h"
#include "JWebTop/tests/TestUtil.h"
#include "JWebTopCommons.h"

using namespace std;

JWebTopClient::JWebTopClient()
: is_closing_(false) {
}

JWebTopClient::~JWebTopClient() {
}

extern bool g_single_process;
// ��ʱ��¼����������Ϣ��������JWebTopBrowser��JWebTopClient���ݲ���������ΪJWebTopClient��ȫ��Ψһʵ����ʹ�ú��ÿ�
extern JWebTopConfigs  tmpConfigs;
void JWebTopClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	browser_list_.push_back(browser);// ��¼���Ѿ������Ĵ�����
	renderBrowserWindow(browser, tmpConfigs);
#ifdef JWebTopLog
	writeLog("===JWebTopClient-------------------------OnAfterCreated\r\n");
	CefWindowInfo windowInfo;
	windowInfo.SetAsPopup(NULL, "cef_debug");
	browser->GetHost()->ShowDevTools(windowInfo, new DEBUG_Handler(), CefBrowserSettings(), CefPoint());
#endif

	CefMessageRouterConfig config;
	message_router_ = CefMessageRouterBrowserSide::Create(config);
	// Register handlers with the router.
	jc::CreateMessageHandlers(message_handler_set_);
	MessageHandlerSet::const_iterator it = message_handler_set_.begin();
	for (; it != message_handler_set_.end(); ++it)
		message_router_->AddHandler(*(it), false);
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

bool JWebTopClient::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed destription of this
	// process.
	if (browser_list_.size() == 1) {
		// Set a flag to indicate that the window close should be allowed.
		is_closing_ = true;
	}

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void JWebTopClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();
	// Remove from the list of existing browsers.
	BrowserList::iterator bit = browser_list_.begin();
	for (; bit != browser_list_.end(); ++bit) {
		if ((*bit)->IsSame(browser)) {
			browser_list_.erase(bit);
			break;
		}
	}
	if (browser_list_.empty()) {// ���������б��Ѿ�Ϊ�գ���ô�˳����������Ϣѭ��
		CefQuitMessageLoop();
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

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL " << std::string(failedUrl) <<
		" with error " << std::string(errorText) << " (" << errorCode <<
		").</h2></body></html>";
	frame->LoadString(ss.str(), failedUrl);
}

void JWebTopClient::OnLoadEnd(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	int httpStatusCode){
	// ���JWebTop�����handler���Ժ�close�������ŵ�OnAfterCreated�У�ҳ�����¼��غ����ͱ����ᶪʧ��
	stringstream extensionCode;
	CefRefPtr<CefBrowserHost> host = browser->GetHost();
	HWND hwnd = host->GetWindowHandle();
	extensionCode << "if(!JWebTop)JWebTop={};";
	extensionCode << "JWebTop.handler=" << (LONG)hwnd << ";" << endl;
	extensionCode << "JWebTop.cefQuery = function(ajson){ window.cefQuery({ request:JSON.stringify(ajson) }) }; " << endl;// ��װ��window.cefQuery����
	if (!g_single_process){	// �����ģʽ�£���Ҫ��������Ϣ�ķ�ʽע����Ҫ����HWND��ȡBorwser�ĺ�����JWebTop����
		// close(handler);// �رմ���
		extensionCode << "JWebTop.close=function(handler){JWebTop.cefQuery({m:'close',handler:(handler?handler:JWebTop.handler)})};" << endl;
		//loadUrl(url, handler);//������ҳ��urlΪ��ҳ·��
		extensionCode << "JWebTop.loadUrl=function(url,handler){JWebTop.cefQuery({m:'loadUrl',url:url,handler:(handler?handler:JWebTop.handler)})};" << endl;
		//reload(handler);//���¼��ص�ǰҳ��
		extensionCode << "JWebTop.reload=function(handler){JWebTop.cefQuery({m:'reload',handler:(handler?handler:JWebTop.handler)})};" << endl;
		//reloadIgnoreCache(handler);//���¼��ص�ǰҳ�沢���Ի���
		extensionCode << "JWebTop.reloadIgnoreCache=function(handler){JWebTop.cefQuery({m:'reloadIgnoreCache',handler:(handler?handler:JWebTop.handler)})};" << endl;
		//showDev(handler);//�򿪿����߹���
		extensionCode << "JWebTop.showDev=function(){JWebTop.cefQuery({m:'showDev',handler:JWebTop.handler})};" << endl;
	}
	// ҳ����غ󣬴���JWebTopReady��Ϣ
	extensionCode << "var e = new CustomEvent('JWebTopReady');" << "setTimeout('dispatchEvent(e);',0);" << endl;
	browser->GetMainFrame()->ExecuteJavaScript(CefString(extensionCode.str()), "", 0);
}

void JWebTopClient::CloseAllBrowsers(bool force_close) {
	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI, base::Bind(&JWebTopClient::CloseAllBrowsers, this, force_close));
		return;
	}
	if (browser_list_.empty()) return;
	BrowserList::const_iterator it = browser_list_.begin();
	for (; it != browser_list_.end(); ++it)
		(*it)->GetHost()->CloseBrowser(force_close);
}