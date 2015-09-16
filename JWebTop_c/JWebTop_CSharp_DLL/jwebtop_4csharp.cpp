#include "jwebtop_4csharp.h"
#include "common/util/StrUtil.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;
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

// ��wstringתΪchar*���ظ�csharp��
char* wstring2char(wstring ws){
	string s = jw::w2s(ws);
	char* c;
	const int len = s.length();
	c = new char[len + 1];
	strcpy(c, s.c_str());
	return c;
}

EXPORT void WINAPI DelPtrInDLL(void *p){
	//writeLog(L"׼��ɾ��ָ��\r\n");
	delete p;
	//writeLog(L"ָ���Ѿ�ɾ��\r\n");
}
EXPORT long WINAPI nCreateJWebTop(LPTSTR processPath, LPTSTR configFile){
	writeLog(L"processPath="); writeLog(processPath); writeLog(L"\r\n");
	writeLog(L"configFile ="); writeLog(configFile); writeLog(L"\r\n");
	return CreateJWebTop(processPath, configFile);
}
EXPORT void WINAPI nExitJWebTop(){
	ExitJWebTop();
}
EXPORT long WINAPI nCreateJWebTopBrowser(LPTSTR jWebTopConfigJSON){
	return CreateJWebTopBrowser(jWebTopConfigJSON);
}
EXPORT void WINAPI nCloseJWebTopBrowser(long browserHWnd){
	CloseJWebTopBrowser(browserHWnd);
}
EXPORT char* WINAPI nJWebTopExecuteJSWait(long browserHWnd, LPTSTR script){
	wstring v=JWebTopExecuteJSWait(browserHWnd, script);
	return wstring2char(v);
}
EXPORT void WINAPI nJWebTopExecuteJSNoWait(long browserHWnd, LPTSTR script){
	JWebTopExecuteJSNoWait(browserHWnd, script);
}
EXPORT char* WINAPI nJWebTopExecuteJSONWait(long browserHWnd, LPTSTR json){
	wstring v = JWebTopExecuteJSONWait(browserHWnd, json);
	return wstring2char(v);
}
EXPORT void WINAPI nJWebTopExecuteJSONNoWait(long browserHWnd, LPTSTR json){
	JWebTopExecuteJSONNoWait(browserHWnd, json);
}

//char* string2char(string s){
//	char* c;
//	const int len = s.length();
//	c = new char[len + 1];
//	strcpy(c, s.c_str());
//	return c;
//}
//EXPORT LPTSTR WINAPI CovWString(LPTSTR v){
//	wstring ws(v);
//	ws.append(L"___��DLL�д������");
//	return LPTSTR(ws.c_str());
//}
//EXPORT int WINAPI CovWString2(LPTSTR v){
//	MessageBox(NULL, v, L"CovWString2", 0);
//	return lstrlenW(v);
//}
//
//EXPORT int WINAPI CovWString3(LPTSTR v1, LPTSTR v2){
//	//MessageBox(NULL, v, L"CovWString3", 0);
//	return lstrlenW(v1) + lstrlenW(v2);
//}
//EXPORT char* WINAPI CovWString4(LPTSTR v){
//	writeLog(L"��������="); writeLog(v); writeLog(L"\r\n");// ����c#�˶������ַ���Ϊunicode������v��utf16����
//	wstring ws(v);
//	ws.append(L"__��wstring�����˴���");
//	string s = jw::w2s(ws);// ת��Ϊutf-8�Ļ���ֱ�Ӿ�������
//	return string2char(s);
//}
//EXPORT void WINAPI CovWString5(
//	LPTSTR outsb,
//	LPTSTR param){
//	realloc(outsb, 100);
//	lstrcpy(outsb, param);
//}
//EXPORT int WINAPI CovString(char* v){
//	return 1234;
//}
//EXPORT char* WINAPI CovString2(char* v){
//	writeLog(L"v  ="); writeLog(v); writeLog(L"\r\n");
//	string s(v);
//	s.append("__����DLL������");
//	return string2char(s);
//}
//EXPORT int WINAPI Calc(int v){
//	return v * 3;
//}