#ifndef CEF_JWEBTOP_CONFIGS_H_
#define CEF_JWEBTOP_CONFIGS_H_
#include <string>
#include "include/cef_client.h"
#include "include/internal/cef_types.h"
class
	JWebTopConfigs{
public:
	// [BASE]С�ڣ�JWebTop������ã�������[BASE]С����
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

	// [CEF]С�ڣ�cef���ò�����cef��ز���ֻ��Ӧ������ʱ�����ã���������[CEF]С����
	int single_process = 1;                      // �Ƿ�ʹ�õ�����ģʽ��JWebTopĬ��ʹ�á�CEFĬ�ϲ�ʹ�õ�����ģʽ
	CefString user_data_path = L"data/jwebtop";  // �û����ݱ���Ŀ¼
	CefString cache_path = L"data/jwebtop";      // ������������ݵı���Ŀ¼
	int persist_session_cookies;                 // �Ƿ���Ҫ�־û��û�cookie���ݣ���Ҫ����Ϊtrue����Ҫͬʱָ��cache_path��
	CefString user_agent = L"JWebTop2.0";        // HTTP�����е�user_agent,CEFĬ����Chorminum��user agent
	CefString locale = "zh-CN";                  // CEFĬ����en-US
	int log_severity = LOGSEVERITY_DISABLE;      // ָ����־�������ȡֵ[0,1,2,3,4,99]Ĭ�ϲ����(99)
	CefString log_file;                          // ָ������ʱ����־�ļ���Ĭ��Ϊ"debug.log"������ر���־���������־
	CefString resources_dir_path;                // ָ��cef��Դ�ļ��� cef.pak��devtools_resources.pak����Ŀ¼��Ĭ�ϴӳ�������Ŀ¼ȡ
	CefString locales_dir_path;                  // ָ��cef���ػ���Դ(locales)Ŀ¼��Ĭ��ȥ��������Ŀ¼�µ�localesĿ¼
	int ignore_certificate_errors;               // �Ƿ����SSL֤�����
	int remote_debugging_port;                   // Զ�̵��Զ˿ڣ�ȡֵ��Χ[1024-65535]

	int no_sandbox = 1;                          // �Ƿ�ʹ��ɳ��ģʽ��JWebTopĬ�ϲ�ʹ�á�CEFĬ��ʹ�á��˲�����JWebTop�в���������Ϊʹ�ã��������ɲ���JWebTop JS����ʹ��
	///// ����cef�����ݲ�֧������
	//CefString product_version;                 // ���ָ����user_agent���˲����ᱻ����,CEFĬ����Chorminum�İ汾��
	//int pack_loading_disabled;// ��ֹ��resources_dir_path��locales_dir_path��ȡ���ݣ�תΪ��CefApp::GetResourceBundleHandler()��ȡ
	//CefString browser_subprocess_path;
	//int multi_threaded_message_loop;
	//int windowless_rendering_enabled;
	//int command_line_args_disabled;
	//cef_string_t javascript_flags;
	//int uncaught_exception_stack_size;// ����δ֪�쳣ʱ�Ķ�ջ��ȡ�ָ��Ϊ0ʱ(Ĭ��ֵ)�����ᴥ��OnUncaughtException() 
	//int context_safety_implementation;
	//cef_color_t background_color;
	//cef_string_t accept_language_list;// HTTPͷ"Accept-Language"���Զ��ŷָ�����������б�Ĭ��Ϊ "en-US,en"

	JWebTopConfigs(){}
	~JWebTopConfigs(){}

	// �������·����ȡ����·��
	// ���relativePath�Ѿ��Ǿ���·������ֱ�ӷ���
	// ���򽫷���appPath+relativePath
	CefString getAbsolutePath(std::wstring relativePath);

	// �õ���������Ŀ¼
	static std::wstring GetExePath();
	// ���ݴ���Ĳ�����ȡ�����ļ�·��(Ŀǰ��Ҫ���ڴ��������ΪNULL����ַ��������)
	static std::wstring JWebTopConfigs::getAppDefFile(LPCTSTR lpCmdLine);
	// �����ݶ����ļ���ȡ
	static JWebTopConfigs loadConfigs(std::wstring appDefFile);
};


#endif