#include "JWebTopContext.h"

#include <sstream>
#include <string>
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "JWebTop/wndproc/JWebTopWndProc.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
#include "JWebTopCommons.h"

using namespace std;
extern CefSettings settings;              // CEFȫ������
namespace jw{
	JWebTopConfigs * g_configs;  // Ӧ������ʱ�ĵ�һ�����ñ���
	
	namespace ctx{
		BrowserList browser_list_;

		JWebTopConfigs * getDefaultConfigs(){
			return g_configs;
		}
		void setDefaultConfigs(JWebTopConfigs * configs){
			g_configs = configs;
		}

		int getBrowserCount(){
			return browser_list_.size();
		}
		void addBrowser(CefRefPtr<CefBrowser> browser) {
			browser_list_.push_back(browser);// ��¼���Ѿ������Ĵ�����
		}

		void removeBrowser(CefRefPtr<CefBrowser> browser) {
			// Remove from the list of existing browsers.
			BrowserList::iterator bit = browser_list_.begin();
			for (; bit != browser_list_.end(); ++bit) {
				if ((*bit)->IsSame(browser)) {
					browser_list_.erase(bit);
					break;
				}
			}
#ifdef JWebTopLog
			wstringstream log;
			log << L"JWebTopClient::OnBeforeClose �д�[" << browser_list_.size() << L"]��"
				<< L"URL��ַ��" << browser->GetMainFrame()->GetURL().ToWString().c_str();
			writeLog(log.str());
#endif
			if (!settings.multi_threaded_message_loop && browser_list_.empty()) {
				// All browser windows have closed. Quit the application message loop.
				CefQuitMessageLoop();
			}
		}


		void CloseAllBrowsers(bool force_close) {
			//if (!CefCurrentlyOn(TID_UI)) {
			//	MessageBox(NULL, L"ֻ����UI����ִ��ȫ���رյĲ���", L"����", 0);
			//	// Execute on the UI thread.
			//	//CefPostTask(TID_UI, base::Bind(&CloseAllBrowsers, this, force_close));
			//	CefPostTask(TID_UI, base::Bind(&CloseAllBrowsers, this, force_close));
			//	return;
			//}
			if (browser_list_.empty()) return;
			BrowserList::const_iterator it = browser_list_.begin();
			for (; it != browser_list_.end(); ++it)
				(*it)->GetHost()->CloseBrowser(force_close);
		}
	}
}
