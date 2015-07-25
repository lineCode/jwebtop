#include "JWebTopJSHanlder.h"
#include "include/cef_app.h"
#include "JJH_Windows.h"

// ��ȡ������Ӧ�Ĵ��ھ����Ĭ�ϴ����һ��������ȡ�����û��֪�����һ����������object��handler��ȡ
HWND getHWND(CefRefPtr<CefV8Value> object/*JS����*/, const CefV8ValueList& arguments/*���������б�*/
	, CefV8ValueList::size_type lastIdx/*���һ�������ĵ�ַ*/){
	LONG hWnd = 0;
	if (arguments.size() > lastIdx){
		hWnd = static_cast<LONG>(arguments[lastIdx]->GetIntValue());
	}
	else{
		CefRefPtr<CefV8Value> v = object->GetValue("handler");
		hWnd = v->GetIntValue();
	}
	return HWND(hWnd);
}

void regist(const CefRefPtr<CefV8Value> jWebTop, const CefString& fn, CefRefPtr<CefV8Handler> handler){
	jWebTop->SetValue(fn, CefV8Value::CreateFunction(fn, handler), V8_PROPERTY_ATTRIBUTE_NONE);
}
void regist(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context){
	CefRefPtr<CefV8Value> object = context->GetGlobal();
	// ��������Զ������
	CefRefPtr<CefV8Accessor> accessor;// �б�Ҫ�Ļ���������չ����
	CefRefPtr<CefV8Value> jWebTop = object->CreateObject(accessor);
	object->SetValue("JWebTop", jWebTop, V8_PROPERTY_ATTRIBUTE_NONE);// �Ѵ����Ķ��󸽼ӵ�V8��������
	// 
	regist(jWebTop, "setSize", new JJH_SetSize());
	regist(jWebTop, "move", new JJH_Move());
	regist(jWebTop, "setTitle", new JJH_SetTitle());
	regist(jWebTop, "getTitle", new JJH_GetTitle());
}