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


#define IDC_CEFCLIENT                   109

#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library is currently built with VS2013. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

bool g_single_process;
HINSTANCE g_instance;
JWebTopConfigs * g_configs;  // Ӧ������ʱ�ĵ�һ�����ñ���
JWebTopConfigs * tmpConfigs; // �����������ڶ���������й���ı���

// Ӧ�ó������
int startJWebTop(HINSTANCE hInstance/*��ǰӦ�õ�ʵ��*/, LPTSTR lpCmdLine) {
	g_instance = hInstance;
//#ifdef JWebTopJNI
//	CefMainArgs main_args(::GetModuleHandle(NULL));
//#else
	CefMainArgs main_args; // �ṩCEF�����в���
//#endif
	CefSettings settings;             // CEFȫ������
	// ��ȡ����������Ϣ
	tmpConfigs = JWebTopConfigs::parseCmdLine(lpCmdLine);
	if (g_configs == NULL)g_configs = tmpConfigs;
	
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

#ifdef JWebTopLog
	settings.log_severity = LOGSEVERITY_VERBOSE;
#endif
	void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#else
	settings.no_sandbox = tmpConfigs->no_sandbox;
#endif	
//#ifdef JWebTopJNI
//	settings.single_process = 1;
//	settings.multi_threaded_message_loop = 1;
//#endif
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
//#ifndef JWebTopJNI
	CefRunMessageLoop();// ����CEF��Ϣ������֪��CefQuitMessageLoop()����������
	CefShutdown();      // �ر�CEF
//#endif
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