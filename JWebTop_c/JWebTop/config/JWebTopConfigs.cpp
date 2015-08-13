#include "JWebTopConfigs.h"

#include <Windows.h>
#include <ShellAPI.h> 
#include <iostream> 
#include <sstream> 
#include <strsafe.h>
#include "common/util/StrUtil.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;

// �����ļ�����ȡ��·��
wstring GetFilePath(wstring appDefFile){
	const int BUFSIZE = 4096;
	TCHAR  buffer[BUFSIZE] = TEXT("");
	GetFullPathName(LPCWSTR(appDefFile.data()), BUFSIZE, buffer, NULL);// ��ȡ�ļ��ľ���·��
	wstring pp(buffer);
	int p = pp.find_last_of('\\');
	if (p != -1)pp = pp.substr(0, p + 1);
	return pp;
}
CefString JWebTopConfigs::getAbsolutePath(std::wstring relativePath){
	if (relativePath.find(L":") == -1){// ���ָ����·�������·��
		std::wstring _path(appPath);
		relativePath = _path.append(relativePath);
	}
	return CefString(relativePath);
}

// �õ���������Ŀ¼
std::wstring JWebTopConfigs::GetExePath(){
	TCHAR   szPath[1000];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	std::wstring path(szPath);
	path = path.substr(0, path.find_last_of('\\') + 1);
	return path;
}

// ���ݴ���Ĳ�����ȡ�����ļ�·��(Ŀǰ��Ҫ���ڴ��������ΪNULL����ַ��������)
std::wstring JWebTopConfigs::getAppDefFile(LPCTSTR lpCmdLine){
	if (lpCmdLine == NULL || lstrlen(lpCmdLine) == 0){// δָ�������ļ���������Ƿ�ͨ����������ָ����appDefFile
		return GetExePath() + L"index.app";// ȡ��������Ŀ¼�µ�index.app�ļ�;
	}
	else{// ȡ����������ΪappDefFile·��
		std::wstring tmp(lpCmdLine);
		if (tmp.find(L":") == -1){// ���ָ����·�������·��
			return GetExePath() + lpCmdLine;
		}
		return wstring(lpCmdLine);
	}
}

JWebTopConfigs * JWebTopConfigs::loadConfigs(std::wstring appDefFile){
#ifdef JWebTopLog
	writeLog(L"appDefFile=====" + appDefFile + L"\r\n");
#endif
	JWebTopConfigs * configs =new JWebTopConfigs();
	if (appDefFile.size() == 0)return configs;                                                   // ���δָ�������ļ���null����ָ�룩 
	LPCTSTR path = appDefFile.data();
	if (_waccess(path, 0) != 0)return configs;                                                   // ����ļ�������
	configs->appDefFile = CefString(appDefFile);
	configs->appPath = GetFilePath(appDefFile);
	TCHAR url[1000], name[100], iconPath[1000];
	GetPrivateProfileString(L"BASE", L"url", NULL, url, 1000, appDefFile.data());
	configs->url = CefString(url);                                                                // ��Ҫ�򿪵ĵ�ַ 
	GetPrivateProfileString(L"BASE", L"name", NULL, name, 100, appDefFile.data());
	configs->name = CefString(name);                                                              // ����������������� 
	GetPrivateProfileString(L"BASE", L"icon", NULL, iconPath, 1000, appDefFile.data());
	configs->icon = CefString(iconPath);                                                          // ���������ͼ�� 
	// ��ʵ���������ͨ����ȡ�ַ�����ת��Ϊint�ķ�ʽ���ж�
	configs->w = GetPrivateProfileInt(L"BASE", L"width", -1, appDefFile.data());
	configs->h = GetPrivateProfileInt(L"BASE", L"height", -1, appDefFile.data());
	configs->x = GetPrivateProfileInt(L"BASE", L"x", -1, appDefFile.data());
	configs->y = GetPrivateProfileInt(L"BASE", L"y", -1, appDefFile.data());
	configs->enableDrag = 1 == GetPrivateProfileInt(L"BASE", L"enableDrag", 1, appDefFile.data());        // Ĭ�Ͽ���ͨ��ҳ������϶�
	configs->disableRefresh = 0 == GetPrivateProfileInt(L"BASE", L"disableRefresh", 0, appDefFile.data());// Ĭ�Ͻ�ֹˢ��
	configs->enableDebug = 0 == GetPrivateProfileInt(L"BASE", L"disableDevelop", 1, appDefFile.data());   // Ĭ�ϲ����Ե���
	configs->enableResize = 1 == GetPrivateProfileInt(L"BASE", L"enableResize", 1, appDefFile.data());    // Ĭ�Ͽ��Ըı��С
	//disableTransparent = 1==GetPrivateProfileInt(L"BASE", L"disableTransparent", 0, appDefFile.data());
	//hasBorder = 1==GetPrivateProfileInt(L"BASE", L"hasBorder", 0, appDefFile.data());
	configs->max = GetPrivateProfileInt(L"BASE", L"max", 0, appDefFile.data());
	configs->dwStyle = GetPrivateProfileInt(L"BASE", L"style", 0, appDefFile.data());
	configs->dwExStyle = GetPrivateProfileInt(L"BASE", L"exStyle", 0, appDefFile.data());

	// cef
	configs->single_process = GetPrivateProfileInt(L"CEF", L"single_process", configs->single_process, appDefFile.data());             // �Ƿ�ʹ�õ�����ģʽ��JWebTopĬ��ʹ�á�CEFĬ�ϲ�ʹ�õ�����ģʽ
	TCHAR user_data_path_[1000];
	GetPrivateProfileString(L"CEF", L"user_data_path", configs->user_data_path.ToWString().c_str(), user_data_path_, 1000, appDefFile.data());
	configs->user_data_path = CefString(user_data_path_); // �û����ݱ���Ŀ¼
	TCHAR cache_path_[1000];
	GetPrivateProfileString(L"CEF", L"cache_path", configs->cache_path.ToWString().c_str(), cache_path_, 1000, appDefFile.data());
	configs->cache_path = CefString(cache_path_); // ������������ݵı���Ŀ¼
	configs->persist_session_cookies = GetPrivateProfileInt(L"CEF", L"single_process", configs->persist_session_cookies, appDefFile.data());             // �Ƿ���Ҫ�־û��û�cookie���ݣ���Ҫ����Ϊtrue����Ҫͬʱָ��cache_path��
	TCHAR user_agent_[1000];
	GetPrivateProfileString(L"CEF", L"user_agent", configs->user_agent.ToWString().c_str(), user_agent_, 1000, appDefFile.data());
	configs->user_agent = CefString(user_agent_);  // HTTP�����е�user_agent,CEFĬ����Chorminum��user agent
	TCHAR locale_[1000];
	GetPrivateProfileString(L"CEF", L"locale", configs->locale.ToWString().c_str(), locale_, 1000, appDefFile.data());
	configs->locale = CefString(locale_);  // CEFĬ����en-US
	configs->log_severity = GetPrivateProfileInt(L"CEF", L"log_severity", configs->log_severity, appDefFile.data());// ָ����־�������Ĭ�ϲ����(disable),����ȡֵ��verbose,info,warning,error,error-report
	TCHAR log_file_[1000];
	GetPrivateProfileString(L"CEF", L"log_file_", configs->log_file.ToWString().c_str(), log_file_, 1000, appDefFile.data());
	configs->log_file = CefString(log_file_);  // ָ������ʱ����־�ļ���Ĭ��Ϊ"debug.log"������ر���־���������־
	TCHAR resources_dir_path_[1000];
	GetPrivateProfileString(L"CEF", L"resources_dir_path", configs->resources_dir_path.ToWString().c_str(), resources_dir_path_, 1000, appDefFile.data());
	configs->resources_dir_path = CefString(resources_dir_path_);  // ָ��cef��Դ�ļ��� cef.pak��devtools_resources.pak����Ŀ¼��Ĭ�ϴӳ�������Ŀ¼ȡ
	TCHAR locales_dir_path_[1000];
	GetPrivateProfileString(L"CEF", L"locales_dir_path", configs->locales_dir_path.ToWString().c_str(), locales_dir_path_, 1000, appDefFile.data());
	configs->locales_dir_path = CefString(locales_dir_path_);// ָ��cef���ػ���Դ(locales)Ŀ¼��Ĭ��ȥ��������Ŀ¼�µ�localesĿ¼
	configs->ignore_certificate_errors = GetPrivateProfileInt(L"CEF", L"ignore_certificate_errors", configs->ignore_certificate_errors, appDefFile.data());             // �Ƿ����SSL֤�����
	configs->remote_debugging_port = GetPrivateProfileInt(L"CEF", L"remote_debugging_port", configs->remote_debugging_port, appDefFile.data());                 // Զ�̵��Զ˿ڣ�ȡֵ��Χ[1024-65535]
	return configs;
}

// �������ж�ȡ
JWebTopConfigs * JWebTopConfigs::parseCmdLine(LPTSTR szCmdLine){
	if (szCmdLine == NULL || lstrlen(szCmdLine) == 0){
		szCmdLine = LPTSTR(JWebTopConfigs::getAppDefFile(szCmdLine).c_str());// û��ָ�������в���ʱ��Ĭ�������ļ�����
	}
	if (szCmdLine[0] != ':'){// ����:��ͷ����Ϊ����ͨ���ļ�
		return JWebTopConfigs::loadConfigs(JWebTopConfigs::getAppDefFile(szCmdLine));
	}
	int argc = 0;
	LPTSTR * args = CommandLineToArgvW(szCmdLine, &argc);
	JWebTopConfigs * configs = loadConfigs(JWebTopConfigs::getAppDefFile(args[7]));
	//// args[0]===������š�:��
	configs->msgWin = atol(wch2chr(args[1]));
	configs->parentWin = atol(wch2chr(args[2]));
	int tmpi = atoi(wch2chr(args[3]));
	if (tmpi != -1)configs->x = tmpi;
	tmpi = atoi(wch2chr(args[4]));
	if (tmpi != -1)configs->y =tmpi;
	tmpi = atoi(wch2chr(args[5]));
	if (tmpi != -1)configs->w = tmpi;
	tmpi = atoi(wch2chr(args[6]));
	if (tmpi != -1)configs->h = tmpi;
	LPTSTR  url = args[8];
	if (url[0] != ':')configs->url = CefString(url);
	url = args[9];
	if (url[0] != ':')configs->icon = CefString(url);
	if (argc > 10)configs->name = CefString(args[10]);
	return configs;
}