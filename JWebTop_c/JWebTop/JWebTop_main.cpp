// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <windows.h>

#include "browser/JWebTopApp.h"
#include "include/cef_sandbox_win.h"
#include "JWebTop/winctrl/JWebTopConfigs.h"
#ifdef JWebTopLog
#include "JWebTop/tests/TestUtil.h"
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
int startJWebTop(HINSTANCE hInstance/*��ǰӦ�õ�ʵ��*/, LPTSTR appDefFile, long parentWin
	// ���²������滻appfile�е���Ӧ����
	, LPTSTR url       // Ҫ�򿪵����ӵ�ַ
	, LPTSTR title     // ��������
	, LPTSTR icon      // ����ͼ��
	, int x, int y     // �������Ͻ�����,��ֵΪ-1ʱ�����ô˱���		 
	, int w, int h     // ���ڵĿ��ߣ���ֵΪ-1ʱ�����ô˱���	
	) {
	g_instance = hInstance;
	#ifdef JWebTopJNI
		CefMainArgs main_args(::GetModuleHandle(NULL));
	#else
		CefMainArgs main_args(g_instance); // �ṩCEF�����в���
	#endif
	CefSettings settings;             // CEFȫ������
	// ��ȡ����������Ϣ
	tmpConfigs = JWebTopConfigs::loadConfigs(JWebTopConfigs::getAppDefFile(appDefFile));
	if (g_configs == NULL)g_configs = tmpConfigs;
	if (url != NULL){
		tmpConfigs->url = CefString(url);
	}
	if (title != NULL)tmpConfigs->name = CefString(title);
	if (icon != NULL)tmpConfigs->icon = CefString(icon);
	if (x != -1)tmpConfigs->x = x;
	if (y != -1)tmpConfigs->y = y;
	if (w != -1)tmpConfigs->w = w;
	if (h != -1)tmpConfigs->h = h;

	tmpConfigs->parentWin = parentWin;
	// ��CEF����һЩ����
	settings.single_process = tmpConfigs->single_process;                      // �Ƿ�ʹ�õ�����ģʽ��JWebTopĬ��ʹ�á�CEFĬ�ϲ�ʹ�õ�����ģʽ
	CefString(&settings.user_data_path) = tmpConfigs->user_data_path;          // �û����ݱ���Ŀ¼
	CefString(&settings.cache_path) = tmpConfigs->cache_path;                 // ������������ݵı���Ŀ¼
	settings.persist_session_cookies = tmpConfigs->persist_session_cookies;    // �Ƿ���Ҫ�־û��û�cookie���ݣ���Ҫ����Ϊtrue����Ҫͬʱָ��cache_path��
	CefString(&settings.user_agent) = tmpConfigs->user_agent;                 // HTTP�����е�user_agent,CEFĬ����Chorminum��user agent
	CefString(&settings.locale) = tmpConfigs->locale;                          // CEFĬ����en-US
	settings.log_severity = cef_log_severity_t(tmpConfigs->log_severity);      // ָ����־�������Ĭ�ϲ����(disable),����ȡֵ��verbose,info,warning,error,error-report
	CefString(&settings.log_file) = tmpConfigs->log_file;                      // ָ������ʱ����־�ļ���Ĭ��Ϊ"debug.log"������ر���־���������־
	CefString(&settings.resources_dir_path) = tmpConfigs->resources_dir_path;  // ָ��cef��Դ�ļ��� cef.pak��devtools_resources.pak����Ŀ¼��Ĭ�ϴӳ�������Ŀ¼ȡ
	CefString(&settings.locales_dir_path) = tmpConfigs->locales_dir_path;      // ָ��cef���ػ���Դ(locales)Ŀ¼��Ĭ��ȥ��������Ŀ¼�µ�localesĿ¼
	settings.ignore_certificate_errors = tmpConfigs->ignore_certificate_errors;// �Ƿ����SSL֤�����
	settings.remote_debugging_port = tmpConfigs->remote_debugging_port;        // Զ�̵��Զ˿ڣ�ȡֵ��Χ[1024-65535]

	void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#else
	settings.no_sandbox = tmpConfigs->no_sandbox;
#endif	
	settings.multi_threaded_message_loop = 1;
	CefRefPtr<JWebTopApp> app(new JWebTopApp);// �������ڼ����Ķ�������ͨ����app��OnContextInitialized���������ʵ��

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
	if (settings.multi_threaded_message_loop){// �����ж�������Ϣѭ��
		MSG msg;
		HACCEL	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CEFCLIENT));
		// Run the application message loop.
		while (GetMessage(&msg, NULL, 0, 0)) {
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	else{
		CefInitialize(main_args, settings, app.get(), sandbox_info);// ��ʼ��cef
		CefRunMessageLoop();// ����CEF��Ϣ������֪��CefQuitMessageLoop()����������
		// ������������Լ�������Ϣѭ���ķ�ʽ�����ܱ�CefRunMessageLoop()�Բ���Ǻô������Լ������̼߳��������������Ȼ�޷����java�߳���cef��Ϣ��ͻ�����⣩
		//while (runable){
		//	CefDoMessageLoopWork();
		//	Sleep(30);
		//}
	}
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
	startJWebTop(hInstance, lpCmdLine, 0, NULL, NULL, NULL, -1, -1, -1, -1);
	return 0;
}