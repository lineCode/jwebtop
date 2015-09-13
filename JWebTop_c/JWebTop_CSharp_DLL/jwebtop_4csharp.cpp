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

EXPORT LPTSTR WINAPI CovWString(LPTSTR v){
	wstring ws(v);
	ws.append(L"___��DLL�д������");
	return LPTSTR(ws.c_str());
}
EXPORT int WINAPI CovWString2(LPTSTR v){
	MessageBox(NULL, v, L"CovWString2", 0);
	return lstrlenW(v);
}
EXPORT int WINAPI CovWString3(LPTSTR v){
	MessageBox(NULL, v, L"CovWString3", 0);
	return lstrlenW(v);
}
char* string2char(string s){
	// ����ķ�ʽ�ǶԵ�
	//int len = s.length();
	//char * bs;// ����һ��ָ�벢����ռ䣨ָ�����Ŀռ��ڶ���,���ϵĿռ����ɳ���Ա������ͷŵģ�������Ա���ͷ�,�������ʱ������OS�ͷţ�
	//bs = (char *)malloc(len + 1);// Ϊָ�����ռ�
	//memcpy(bs, s.c_str(), len);
	//return bs;
	 // ����ķ�ʽҲ����
	char* c;
	const int len = s.length();
	c = new char[len + 1];
	strcpy(c, s.c_str());
	return c;
}
EXPORT char* WINAPI CovWString4(LPTSTR v){
	writeLog(L"��������="); writeLog(v); writeLog(L"\r\n");// ����c#�˶������ַ���Ϊunicode������v��utf16����
	wstring ws(v);
	ws.append(L"__��wstring�����˴���");
	string s = jw::w2s(ws);// ת��Ϊutf-8�Ļ���ֱ�Ӿ�������
	return string2char(s);
}
EXPORT int WINAPI CovString(char* v){
	return 1234;
}
EXPORT char* WINAPI CovString2(char* v){
	writeLog(L"v  ="); writeLog(v); writeLog(L"\r\n");
	string s(v);
	s.append("__����DLL������");
	return string2char(s);
}
EXPORT int WINAPI Calc(int v){
	return v * 3;
}