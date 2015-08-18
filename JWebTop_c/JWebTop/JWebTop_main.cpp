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
#include "common/JWebTopMsg.h"
#include "common/task/Task.h"
#include "common/util/StrUtil.h"
#include "common/msgwin/MsgWin.h"
#include "browser/JWebTopClient.h"

#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library is currently built with VS2013. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

bool g_single_process;
JWebTopConfigs * g_configs;  // Ӧ������ʱ�ĵ�һ�����ñ���
JWebTopConfigs * tmpConfigs; // �����������ڶ���������й���ı���
extern HWND g_LocalWinHWnd;  // ���ش�������Ϣ����HWND
extern HWND g_RemoteWinHWnd;  // Զ�̽��̵���Ϣ����HWND
extern CefRefPtr<JWebTopClient> g_handler;// ȫ�ֱ���һ��JWebTopClient����
void waitRemoteProcessTerminate(){
	try{
		DWORD lpdwProcessId;
		GetWindowThreadProcessId(g_RemoteWinHWnd, &lpdwProcessId);
		//HANDLE hHandle = GetProcessHandleFromHwnd(g_RemoteWinHWnd);// �˺�����ͷ�ļ���ȷ�����ĸ�(Header: Use LoadLibrary and GetProcAddress.  Library: Use Oleacc.dll.)
		HANDLE hHandle = OpenProcess(PROCESS_ALL_ACCESS, false, lpdwProcessId);
		wstringstream wss;
		wss << L"��ȡ���̾����pId=" << lpdwProcessId << "��Handle=" << (long)hHandle << ",lastErr=" << GetLastError() << "\r\n";
		writeLog(wss.str());
		WaitForSingleObject(hHandle, INFINITE);
	}
	catch (...){
		writeLog(L"�������쳣.............\t\n");
	}
	jw::sendProcessMsg(g_LocalWinHWnd, WM_COPYDATA_EXIT, L"");// ֪ͨ�����������ڣ�������Ҫ�ر�
}

// Ӧ�ó������
int startJWebTop(HINSTANCE hInstance/*��ǰӦ�õ�ʵ��*/, LPTSTR lpCmdLine) {
	CefMainArgs main_args(hInstance);  // �ṩCEF�����в���
	CefSettings settings;              // CEFȫ������
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

	//settings.single_process = 1;
	if (tmpConfigs->msgWin != 0){
		g_RemoteWinHWnd = (HWND)tmpConfigs->msgWin;
		std::thread waitRemoteProcessThread(waitRemoteProcessTerminate);
		waitRemoteProcessThread.detach();// �ڶ����߳��еȴ�Զ�̽��̵Ľ�����Ϣ
		settings.multi_threaded_message_loop = 1;
	}
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
		jw::createWin(hInstance, lpCmdLine);// �������ش��ڲ�������ǰ�߳�
		// ��MsgWin��PostQuitMessage(0)֮������Ĵ������û�л���ִ���ˣ���������ֱ���˳�
		g_handler->CloseAllBrowsers(true);
		CefQuitMessageLoop();
	}
	else{
		CefRunMessageLoop();// ����CEF��Ϣ������֪��CefQuitMessageLoop()����������
	}
	jw::task::unlockAndClearAll();
	CefShutdown();      // �ر�CEF
	return 0;
}

// ������Ϣ���ڵ�WM_COPYDATA��Ϣ
void jw::msgwin_thread_executeWmCopyData(DWORD msgId, std::wstring msg){
	// ��ʱ����Ҫ���κδ���
}
// ����createWin���лص�
void jw::onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine){
	// ��ʱ����Ҫ���κδ���
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