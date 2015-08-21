// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <windows.h>
#include "browser/JWebTopApp.h"
#include "include/cef_sandbox_win.h"
#include "JWebTop/config/JWebTopConfigs.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
#include "common/task/Task.h"
#include "common/msgwin/MsgWin.h"
#include "browser/JWebTopContext.h"
#include "dllex/JWebTop_DLLEx.h"
#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library is currently built with VS2013. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

extern HWND g_RemoteWinHWnd;               // Զ�̽��̵���Ϣ����HWND
CefSettings settings;                      // CEFȫ������

// Ӧ�ó������
int startJWebTop(HINSTANCE hInstance/*��ǰӦ�õ�ʵ��*/, LPTSTR lpCmdLine) {
	//MessageBox(NULL, L"���ڸ��ӽ��̵��ж�", L"�ж�", 0);
	CefMainArgs main_args(hInstance);  // �ṩCEF�����в���
	wstring taskId;
	// ��ȡ����������Ϣ
	JWebTopConfigs * tmpConfigs = JWebTopConfigs::parseCreateJWebTopCmdLine(lpCmdLine, std::ref(taskId));
	jw::ctx::setDefaultConfigs(tmpConfigs);
	// ��CEF����һЩ����
	settings.single_process = tmpConfigs->single_process;                      // �Ƿ�ʹ�õ�����ģʽ��JWebTopĬ��ʹ�á�CEFĬ�ϲ�ʹ�õ�����ģʽ
	CefString(&settings.user_data_path) = tmpConfigs->user_data_path;          // �û����ݱ���Ŀ¼
	CefString(&settings.cache_path) = tmpConfigs->cache_path;                  // ������������ݵı���Ŀ¼
	settings.persist_session_cookies = tmpConfigs->persist_session_cookies;    // �Ƿ���Ҫ�־û��û�cookie���ݣ���Ҫ����Ϊtrue����Ҫͬʱָ��cache_path��
	CefString(&settings.user_agent) = tmpConfigs->user_agent;                  // HTTP�����е�user_agent,CEFĬ����Chorminum��user agent
	CefString(&settings.locale) = tmpConfigs->locale;                          // CEFĬ����en-US
	settings.log_severity = cef_log_severity_t(tmpConfigs->log_severity);      // ָ����־�������Ĭ�ϲ����(disable),����ȡֵ��verbose,info,warning,error,error-report
	CefString(&settings.log_file) = tmpConfigs->log_file;                      // ָ������ʱ����־�ļ���Ĭ��Ϊ"debug.log"������ر���־���������־
	CefString(&settings.resources_dir_path) = tmpConfigs->resources_dir_path;  // ָ��cef��Դ�ļ��� cef.pak��devtools_resources.pak����Ŀ¼��Ĭ�ϴӳ�������Ŀ¼ȡ
	CefString(&settings.locales_dir_path) = tmpConfigs->locales_dir_path;      // ָ��cef���ػ���Դ(locales)Ŀ¼��Ĭ��ȥ��������Ŀ¼�µ�localesĿ¼
	settings.ignore_certificate_errors = tmpConfigs->ignore_certificate_errors;// �Ƿ����SSL֤�����
	settings.remote_debugging_port = tmpConfigs->remote_debugging_port;        // Զ�̵��Զ˿ڣ�ȡֵ��Χ[1024-65535]

	//settings.single_process = 1;
	if (g_RemoteWinHWnd != NULL){
		jw::dllex::waitForRemoteProcessTerminate();// ����һ���߳���������Զ�̽����Ƿ���ֹ�Ա������ǰ����
		settings.multi_threaded_message_loop = 1;
	}
//#ifdef JWebTopLog
//	settings.log_severity = LOGSEVERITY_VERBOSE;
//#endif
	void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#else
	settings.no_sandbox = tmpConfigs->no_sandbox;
#endif	
	CefRefPtr<JWebTopApp> app(new JWebTopApp);// �������ڼ����Ķ�������ͨ����app��OnContextInitialized���������ʵ��

	CefInitialize(main_args, settings, app.get(), sandbox_info);// ��ʼ��cef
	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);
#ifdef JWebTopLog 
	std::stringstream log;
	log << "CefExecuteProcess exit_code ==" << exit_code << "\r\n";
	writeLog(log.str());
#endif
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return exit_code;
	}
	if (settings.multi_threaded_message_loop == 1){// ����Ǳ�DLL���ã����ַ�ʽֻ�ܽ���һ������		
		jw::createWin(hInstance, LPTSTR(taskId.c_str()));       // �������ش��ڲ�������ǰ�߳�
		// ��MsgWin��PostQuitMessage(0)֮������Ĵ������û�л���ִ���ˣ���������ֱ���˳�
		jw::ctx::CloseAllBrowsers(true);
		CefQuitMessageLoop();
	}
	else{
		CefRunMessageLoop();// ����CEF��Ϣ������֪��CefQuitMessageLoop()����������
	}
	jw::task::unlockAndClearAll();
	CefShutdown();      // �ر�CEF
	return 0;
}

// Ӧ�ó������
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	startJWebTop(hInstance, lpCmdLine);
	return 0;
}