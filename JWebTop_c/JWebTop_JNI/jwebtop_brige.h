#include "include/cef_client.h"

int startJWebTop(HINSTANCE hInstance/*��ǰӦ�õ�ʵ��*/, LPTSTR lpCmdLine, long hwnd
	// ���²������滻appfile�е���Ӧ����
	, LPTSTR url       // Ҫ�򿪵����ӵ�ַ
	, LPTSTR title     // ��������
	, LPTSTR icon      // ����ͼ��
	, int x, int y      // �������Ͻ�����,��ֵΪ-1ʱ�����ô˱���		 
	, int w, int h      // ���ڵĿ��ߣ���ֵΪ-1ʱ�����ô˱���	
	);

//// ��������ص�java����
//CefString  invokeJavaMethod(CefString json);// �����ʵ����JWebTopJSHanlder��JJH_Windows�У�ͨ���ꡰJWebTop_JNI��������