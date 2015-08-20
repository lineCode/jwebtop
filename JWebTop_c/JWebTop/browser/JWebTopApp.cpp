#include "JWebTopApp.h"
#include "JWebTopCommons.h"
namespace jw{
	extern JWebTopConfigs * g_configs;
}
extern HWND g_RemoteWinHWnd;
void JWebTopApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line) {
	command_line->AppendSwitch("--disable-gpu");// ��ֹgpu������gpu������һ���½��̣��������еĵ����Ϸ����ή�����ܣ�
}

void JWebTopApp::OnContextInitialized() {
	if (g_RemoteWinHWnd == NULL){// ������Ǵ�dll���ã���ôֱ�Ӵ��������
		createNewBrowser(jw::g_configs);
	}
	else{// ����Ǵ�dll���ã������������
		//if (!g_configs->url.empty())createNewBrowser(g_configs);
	}
}