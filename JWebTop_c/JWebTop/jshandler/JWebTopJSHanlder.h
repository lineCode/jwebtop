#ifndef CEF_JWEBTOP_JS_HANDLER_H_
#define CEF_JWEBTOP_JS_HANDLER_H_

#include "include/cef_app.h" 

// Ҳ����ͨ��inline��static�ؼ��ֽ�getHWND����ʵ���ڴ�ͷ�ļ��С�
// ��ȡ������Ӧ�Ĵ��ھ����Ĭ�ϴ����һ��������ȡ�����û��֪�����һ����������object��handler��ȡ
HWND getHWND(CefRefPtr<CefV8Value> object/*JS����*/
	, const CefV8ValueList& arguments/*���������б�*/
	, CefV8ValueList::size_type lastIdx/*���һ�������ĵ�ַ*/);

// ע��JWebTop��غ�����V8Context
void regist(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context);
namespace jw{
	namespace js{
		CefRefPtr<CefListValue> parserCopyFile(HWND hWnd, CefRefPtr<CefDictionaryValue> json);
	}
}
#endif  // CEF_JWEBTOP_JS_HANDLER_H_