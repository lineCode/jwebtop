// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <windows.h>

#include "browser/JWebTopApp.h"
#include "include/cef_sandbox_win.h"
#include "JWebTop/winctrl/JWebTopConfigs.h"

#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library is currently built with VS2013. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

HINSTANCE g_hInstance;
LPTSTR    g_lpCmdLine;
bool g_single_process;
JWebTopConfigs * g_configs;  // Ӧ������ʱ�ĵ�һ�����ñ���
JWebTopConfigs * tmpConfigs; // �����������ڶ���������й���ı���
// Ӧ�ó������
int startJWebTop(HINSTANCE hInstance/*��ǰӦ�õ�ʵ��*/,LPTSTR    lpCmdLine) {

	g_hInstance = hInstance;
	g_lpCmdLine = lpCmdLine;
	// Specify CEF global settings here.
	CefSettings settings;
	// ��ȡ����������Ϣ
	JWebTopConfigs tmp = JWebTopConfigs::loadConfigs(JWebTopConfigs::getAppDefFile(lpCmdLine));
	tmpConfigs = &tmp;
	if (g_configs == NULL)g_configs = tmpConfigs;
	// ��CEF����һЩ����
	settings.single_process = tmpConfigs->single_process;                      // �Ƿ�ʹ�õ�����ģʽ��JWebTopĬ��ʹ�á�CEFĬ�ϲ�ʹ�õ�����ģʽ
	CefString(&settings.user_data_path) = tmpConfigs->user_data_path; // �û����ݱ���Ŀ¼
	CefString(&settings.cache_path) = tmpConfigs->cache_path ;    // ������������ݵı���Ŀ¼
	settings.persist_session_cookies = tmpConfigs->persist_session_cookies;               // �Ƿ���Ҫ�־û��û�cookie���ݣ���Ҫ����Ϊtrue����Ҫͬʱָ��cache_path��
	CefString(&settings.user_agent) = tmpConfigs->user_agent ;      // HTTP�����е�user_agent,CEFĬ����Chorminum��user agent
	CefString(&settings.locale) = tmpConfigs->locale;                // CEFĬ����en-US
	settings.log_severity = cef_log_severity_t(tmpConfigs->log_severity);       // ָ����־�������Ĭ�ϲ����(disable),����ȡֵ��verbose,info,warning,error,error-report
	//settings.log_severity = LOGSEVERITY_DISABLE;       // ������־���𣺹ر���־
	CefString(&settings.log_file) = tmpConfigs->log_file;                        // ָ������ʱ����־�ļ���Ĭ��Ϊ"debug.log"������ر���־���������־
	CefString(&settings.resources_dir_path) = tmpConfigs->resources_dir_path;              // ָ��cef��Դ�ļ��� cef.pak��devtools_resources.pak����Ŀ¼��Ĭ�ϴӳ�������Ŀ¼ȡ
	CefString(&settings.locales_dir_path) = tmpConfigs->locales_dir_path;                // ָ��cef���ػ���Դ(locales)Ŀ¼��Ĭ��ȥ��������Ŀ¼�µ�localesĿ¼
	settings.ignore_certificate_errors = tmpConfigs->ignore_certificate_errors;             // �Ƿ����SSL֤�����
	settings.remote_debugging_port = tmpConfigs->remote_debugging_port;                 // Զ�̵��Զ˿ڣ�ȡֵ��Χ[1024-65535]


	//CefString(&settings.cache_path) = L"data/browser"; // ���û���Ŀ¼������WebSQL��IndexDB�����ݿ�λ�ã�
	//settings.single_process = tmpConfigs->single_process;				   // ���ú�browser��render��ϲ�Ϊһ�����̣����ȶ�����
	//g_single_process = settings.single_process == 1;

	void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif

	// Provide CEF with command-line arguments.
	CefMainArgs main_args(hInstance);

	// SimpleApp implements application-level callbacks. It will create the first
	// browser instance in OnContextInitialized() after CEF has initialized.
	CefRefPtr<JWebTopApp> app(new JWebTopApp);

	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	//app->setMainArgs(hInstance);
	int exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return exit_code;
	}


#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = tmpConfigs->no_sandbox;
#endif	
	CefInitialize(main_args, settings, app.get(), sandbox_info);// ��ʼ��cef
	// Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
	CefRunMessageLoop();
	// Shut down CEF.
	CefShutdown();
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