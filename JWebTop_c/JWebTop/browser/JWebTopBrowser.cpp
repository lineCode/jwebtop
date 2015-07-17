#include <string>

#include "JWebTopBrowser.h"
#include "JWebTopCommons.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

void JWebTopBrowser::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line) {
	command_line->AppendSwitch("--disable-gpu");// ��ֹgpu������gpu������һ���½��̣��������еĵ����Ϸ����ή�����ܣ�
}
void JWebTopBrowser::OnContextInitialized() {
	createNewBrowser((LPCTSTR)NULL);
}