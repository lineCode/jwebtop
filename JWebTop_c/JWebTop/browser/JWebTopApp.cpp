#include "JWebTopApp.h"
#include "JWebTopCommons.h"

extern JWebTopConfigs * g_configs;
void JWebTopApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line) {
	command_line->AppendSwitch("--disable-gpu");// ��ֹgpu������gpu������һ���½��̣��������еĵ����Ϸ����ή�����ܣ�
}

void JWebTopApp::OnContextInitialized() {
	if (g_configs->msgWin != 0){// ������Ǵ�dll���ã���ôֱ�Ӵ��������
		createNewBrowser(g_configs);
	}
	else{// ����Ǵ�dll���ã���ôֻ�е�url��Чʱ�Ŵ��������
		if (!g_configs->url.empty())createNewBrowser(g_configs);
	}
}