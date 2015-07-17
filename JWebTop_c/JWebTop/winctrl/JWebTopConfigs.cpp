#include "JWebTopConfigs.h"
#include "JWebTop/tests/TestUtil.h"
using namespace std;

// �����ļ�����ȡ��·��
wstring GetFilePath(wstring appDefFile){
	const int BUFSIZE = 4096;
	TCHAR  buffer[BUFSIZE] = TEXT("");
	GetFullPathName(LPCWSTR(appDefFile.data()),	BUFSIZE,buffer,	NULL);// ��ȡ�ļ��ľ���·��
	wstring pp(buffer);
	int p = pp.find_last_of('\\');
	if (p != -1)pp = pp.substr(0, p + 1);
	return pp;
}
CefString JWebTopConfigs::getAbsolutePath(std::wstring relativePath){
	if (relativePath.find(L":") == -1){// ���ָ����·�������·��
		wstring _path(appPath);
		relativePath = _path.append(relativePath);
	}
	return CefString(relativePath);
}

JWebTopConfigs loadConfigs(std::wstring appDefFile){
#ifdef WebTopLog
	writeLog(L"appDefFile=====" + appDefFile + L"\r\n");
#endif
	JWebTopConfigs configs = JWebTopConfigs();
	if (appDefFile.size() == 0)return configs;                                                   // ���δָ�������ļ���null����ָ�룩 
	LPCTSTR path = appDefFile.data();
	if (_waccess(path, 0) != 0)return configs;                                                   // ����ļ�������
	configs.appDefFile = CefString(appDefFile);
	configs.appPath = GetFilePath(appDefFile);
	TCHAR url[1000], name[100], iconPath[1000];
	GetPrivateProfileString(L"BASE", L"url", NULL, url, 1000, appDefFile.data());
	configs.url = CefString(url);                                                                // ��Ҫ�򿪵ĵ�ַ 
	GetPrivateProfileString(L"BASE", L"name", NULL, name, 100, appDefFile.data());
	configs.name = CefString(name);                                                              // ����������������� 
	GetPrivateProfileString(L"BASE", L"icon", NULL, iconPath, 1000, appDefFile.data());
	configs.icon = CefString(iconPath);                                                          // ���������ͼ�� 
	// ��ʵ���������ͨ����ȡ�ַ�����ת��Ϊint�ķ�ʽ���ж�
	configs.w = GetPrivateProfileInt(L"BASE", L"width", -1, appDefFile.data());
	configs.h = GetPrivateProfileInt(L"BASE", L"height", -1, appDefFile.data());
	configs.x = GetPrivateProfileInt(L"BASE", L"x", -1, appDefFile.data());
	configs.y = GetPrivateProfileInt(L"BASE", L"y", -1, appDefFile.data());
	//enableDrag = GetPrivateProfileInt(L"BASE", L"enableDrag", 0, appDefFile.data());
	configs.disableRefresh = 1 == GetPrivateProfileInt(L"BASE", L"disableRefresh", 0, appDefFile.data());
	configs.enableDebug = 0 == GetPrivateProfileInt(L"BASE", L"disableDevelop", 1, appDefFile.data());
	configs.enableResize = 1 == GetPrivateProfileInt(L"BASE", L"enableResize", 0, appDefFile.data());
	//disableTransparent = 1==GetPrivateProfileInt(L"BASE", L"disableTransparent", 0, appDefFile.data());
	//hasBorder = 1==GetPrivateProfileInt(L"BASE", L"hasBorder", 0, appDefFile.data());
	configs.max = GetPrivateProfileInt(L"BASE", L"max", 0, appDefFile.data());
	configs.dwStyle = GetPrivateProfileInt(L"BASE", L"style", 0, appDefFile.data());
	configs.dwExStyle = GetPrivateProfileInt(L"BASE", L"exStyle", 0, appDefFile.data());
	return configs;
}