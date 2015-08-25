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

extern CefSettings settings;              // CEFȫ������
void regist(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context){

	CefRefPtr<CefV8Value> object = context->GetGlobal();
	// ��������Զ������
	CefRefPtr<CefV8Accessor> accessor;// �б�Ҫ�Ļ���������չ����
	CefRefPtr<CefV8Value> jWebTop = object->CreateObject(accessor);
	object->SetValue("JWebTop", jWebTop, V8_PROPERTY_ATTRIBUTE_NONE);// �Ѵ����Ķ��󸽼ӵ�V8��������

	// 
	regist(jWebTop, "getPos", new JJH_GetPos());//getPos(handler);//��ô���λ�ã�����ֵΪһobject����ʽ����{x:13,y:54}
	regist(jWebTop, "setSize", new JJH_SetSize());//setSize(x, y, handler);//���ô��ڴ�С
	regist(jWebTop, "getSize", new JJH_GetSize());//getSize(handler);//��ô��ڴ�С������ֵΪһobject����ʽ����{width:130,height:54}
	regist(jWebTop, "getScreenSize", new JJH_GetScreenSize());//getScreenSize();//��ȡ��Ļ��С������ֵΪһobject����ʽ����{width:130,height:54}
	regist(jWebTop, "move", new JJH_Move());//move(x, y, handler);//�ƶ�����
	regist(jWebTop, "setBound", new JJH_SetBound());//setBound(x, y,w ,h, handler);// ͬʱ���ô��ڵ�����ʹ�С
	regist(jWebTop, "getBound", new JJH_GetBound());//getBound(handler);//��ȡ���ڵ�λ�úʹ�С������ֵΪһobject����ʽ����{x:100,y:100,width:130,height:54}
	regist(jWebTop, "setTitle", new JJH_SetTitle());//setTitle(title, handler);// ���ô�������
	regist(jWebTop, "getTitle", new JJH_GetTitle());//getTitle(handler);// ��ȡ�������ƣ�����ֵΪһ�ַ���

	regist(jWebTop, "bringToTop", new JJH_BringToTop());  // bringToTop(handler);//�����Ƶ����
	regist(jWebTop, "focus", new JJH_Focus());      	  // focus(handler);//ʹ���ڻ�ý���
	regist(jWebTop, "hide", new JJH_Hide());	          // hide(handler);//���ش���
	regist(jWebTop, "max", new JJH_Max());	              // max(handler);//��󻯴���
	regist(jWebTop, "mini", new JJH_Mini());	          // mini(hander);//��С������
	regist(jWebTop, "restore", new JJH_Restore());	      // restore(handler);//��ԭ���ڣ���Ӧ��hide����
	regist(jWebTop, "setTopMost", new JJH_SetTopMost());  // setTopMost(handler);//�����ö����˺�����bringToTop���������ڴ˺�����ʹ������Զ�ö�������������һ�����ڵ������ö�����
	regist(jWebTop, "setWindowStyle", new JJH_SetWindowStyle());	//setWindowStyle(exStyle, handler);//�߼����������ô��ڶ������ԣ������ö�֮�ࡣ


	regist(jWebTop, "invokeRemote_Wait", new JJH_InvokeRemote_Wait());
	regist(jWebTop, "invokeRemote_NoWait", new JJH_InvokeRemote_NoWait());

	// ������ģʽ�£��ſ��Ը���HWNDֱ�ӻ�ȡBrowerWindowInfo
	// �����ģʽҪͨ����Ϣ�������ݣ��μ�JWebTopClient#OnLoadEnd������ʵ����JWebTopCommons#renderBrowserWindow��
	if (settings.single_process){
		regist(jWebTop, "close", new JJH_Close());            // close(handler);// �رմ���
		regist(jWebTop, "loadUrl", new JJH_LoadUrl());	      //loadUrl(url, handler);//������ҳ��urlΪ��ҳ·��
		regist(jWebTop, "reload", new JJH_Reload());	      //reload(handler);//���¼��ص�ǰҳ��
		regist(jWebTop, "reloadIgnoreCache", new JJH_ReloadIgnoreCache());	//reloadIgnoreCache(handler);//���¼��ص�ǰҳ�沢���Ի���
		regist(jWebTop, "showDev", new JJH_ShowDev());	      //showDev(handler);//�򿪿����߹���
		regist(jWebTop, "enableDrag", new JJH_enableDrag());  // enableDrag(true|false);
		regist(jWebTop, "startDrag", new JJH_startDrag());	  // startDrag();
		regist(jWebTop, "stopDrag", new JJH_stopDrag());	  // stopDrag();
	}
	// ���·���ֻ��ʵ��JWebTopClient#OnLoadEndʵ�֣�����ʵ����JWebTopCommons#renderBrowserWindow��
	//regist(jWebTop, "runApp", new JJH_RunApp());  //runApp(appName,handler);//����һ��app��appNameΪ.app�ļ�·����
}