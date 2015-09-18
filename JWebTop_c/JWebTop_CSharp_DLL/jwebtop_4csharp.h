#ifndef _Included_org_jwebtop_dll4csharp
#define _Included_org_jwebtop_dll4csharp
#include "common_dll/jwebtop_dll.h"
extern "C" {

	typedef char*(CALLBACK *CSharpCallBack)(long browserHWnd, char* s); // ����һ���ص�������csharp��ʵ�ִ˻ص�����
	EXPORT void WINAPI SetCSharpCallback(CSharpCallBack callBack);      // ���ûص�����
	EXPORT void WINAPI DelPtrInDLL(void *p);							// ���ڣ�DLL�����ߣ�ɾ��dll�ж����ָ��
	
	// ���·�����װjwebtop_dll��Ӧ����
	EXPORT long  WINAPI nCreateJWebTop(LPTSTR processPath, LPTSTR configFile);
	EXPORT void  WINAPI nExitJWebTop();
	EXPORT long  WINAPI nCreateJWebTopBrowser(LPTSTR jWebTopConfigJSON);
	EXPORT void  WINAPI nCloseJWebTopBrowser(long browserHWnd);
	EXPORT char* WINAPI nJWebTopExecuteJSWait(long browserHWnd, LPTSTR script);
	EXPORT void  WINAPI nJWebTopExecuteJSNoWait(long browserHWnd, LPTSTR script);
	EXPORT char* WINAPI nJWebTopExecuteJSONWait(long browserHWnd, LPTSTR json);
	EXPORT void  WINAPI nJWebTopExecuteJSONNoWait(long browserHWnd, LPTSTR json);
}
#endif