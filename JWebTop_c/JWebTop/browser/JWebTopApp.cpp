#include "JWebTopApp.h"
#include "JWebTopCommons.h"
#include "JWebTopContext.h"
#include "JWebTop/dllex/JWebTop_DLLEx.h"

void JWebTopApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line) {
	command_line->AppendSwitch("--disable-gpu");// ��ֹgpu������gpu������һ���½��̣��������еĵ����Ϸ����ή�����ܣ�
}

void JWebTopApp::OnContextInitialized() {
	if (jw::dllex::ex()){// ����Ǵ�dll���ã������������
		//if (!g_configs->url.empty())createNewBrowser(g_configs);
	}
	else{// ������Ǵ�dll���ã���ôֱ�Ӵ��������
		createNewBrowser(jw::ctx::getDefaultConfigs());
	}
}