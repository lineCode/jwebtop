#ifndef CEF_JWEBTOP_CONFIGS_H_
#define CEF_JWEBTOP_CONFIGS_H_
#include <string>
#include "include\cef_client.h"
class
	JWebTopConfigs{
public:
	//DWORD dwStyle = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	//DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	DWORD dwStyle = 0;             // ���Ӵ�����ʽ��Ĭ��(0)��Ӧ���κθ��Ӵ�����ʽ
	DWORD dwExStyle = 0;           // ������չ������ʽ��Ĭ��(0)��Ӧ���κθ�����չ������ʽ

	int x = -1, y = -1;            // �������Ͻ�����,��ֵΪ-1ʱ�����ô˱���		 
	int w = -1, h = -1;            // ���ڵĿ��ߣ���ֵΪ-1ʱ�����ô˱���		
	int max;                       // Ĭ���Ƿ����

	bool enableDebug;              // �Ƿ�������ԣ�Ĭ��(false)����ʾ���Թ���
	bool enableResize;             // ����δʵ�֣��Ƿ���Ե������ڴ�С
	bool disableRefresh;           // ����δʵ�֣��Ƿ��ֹˢ��
	bool enableDrag;               // �Ƿ�ͨ��ҳ������϶���Ĭ�Ͽ��ԣ�

	CefString appDefFile;          // ��ǰ���ö�Ӧ�������ļ����п���Ϊnull��
	CefString appPath;             // ��ǰ�����ļ�����Ŀ¼������Ϊnull��
	CefString url;                 // Ҫ�򿪵����ӵ�ַ
	CefString name;                // ��������
	CefString icon;                // ����ͼ��

	JWebTopConfigs(){}
	~JWebTopConfigs(){}

	// �������·����ȡ����·��
	// ���relativePath�Ѿ��Ǿ���·������ֱ�ӷ���
	// ���򽫷���appPath+relativePath
	CefString getAbsolutePath(std::wstring relativePath);
};

// �����ݶ����ļ���ȡ
JWebTopConfigs loadConfigs(std::wstring appDefFile);

#endif