#include "jwebtop_4csharp.h"

CSharpCallBack g_CSharpCallBack;  //�ظ���Ϣ�Ļص�����

/************************************************************************/
/*���ûص�����
/************************************************************************/
EXPORT void WINAPI SetCSharpCallback(CSharpCallBack callBack)
{
	g_CSharpCallBack = callBack;
}

// �ص�c#����
std::wstring invokeByBrowser(long browserHWnd, wstring json){
	return g_CSharpCallBack(browserHWnd, LPTSTR(json.c_str()));
}