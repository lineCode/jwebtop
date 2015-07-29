#include "JWebTopApp.h"
#include "JWebTopCommons.h"

extern JWebTopConfigs * tmpConfigs;
void JWebTopApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line) {
	command_line->AppendSwitch("--disable-gpu");// ��ֹgpu������gpu������һ���½��̣��������еĵ����Ϸ����ή�����ܣ�
}
void JWebTopApp::OnContextInitialized() {
	createNewBrowser(tmpConfigs);
}