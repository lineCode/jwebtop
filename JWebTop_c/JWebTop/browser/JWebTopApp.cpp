#include "JWebTopApp.h"
#include "JWebTopScheme.h"
#include "JWebTopCommons.h"
#include "JWebTopContext.h"
#include "JWebTop/config/JWebTopConfigs.h"
#include "JWebTop/dllex/JWebTop_DLLEx.h"

namespace jw{
	extern JWebTopConfigs * g_configs;  // Ӧ������ʱ�ĵ�һ�����ñ���
}
void JWebTopApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line) {
	command_line->AppendSwitch("--disable-gpu");// ��ֹgpu������gpu������һ���½��̣��������еĵ����Ϸ����ή�����ܣ�
	if (!jw::g_configs->proxyServer.empty()){ 
		command_line->AppendSwitchWithValue("proxy-server", jw::g_configs->proxyServer);// ���ô����������Ҳ���ԣ�
	}
}

void JWebTopApp::OnContextInitialized() {
	jw::jb::RegisterSchemeHandlers();// ע��jwebtop://Э�鵽�����
	if (jw::dllex::ex()){// ����Ǵ�dll���ã������������
		jw::dllex::OnContextInitialized();
		//createNewBrowser(jw::ctx::getDefaultConfigs());
	}
	else{// ������Ǵ�dll���ã���ôֱ�Ӵ��������
		createNewBrowser(jw::ctx::getDefaultConfigs());
	}
}