#ifndef _Included_org_jwebtop_JWebTopdll
#define _Included_org_jwebtop_JWebTopdll
#include <string>
#include <Windows.h>

#define EXPORT __declspec(dllexport)
using namespace std;
/*
 * ���ڵ��ñ��ؽӿڣ�java��c#�ȣ����򣬲�����ִ�н��
 */
wstring invokeByBrowser(long browserHWnd, wstring json);
extern "C" {
	/*
	 * �÷������ڴ���һ�����������
	 * processPath ��ִ�е�JWebTop.exe��ȫ·��������d:\c\JWebTop.exe����ȻJWebTop��������Ϊ�������֡�
	 * configFile  �������ļ���������JWebTop���̣����������̴�����������ڣ���ʹָ����url������
	 *
	 * return �ڱ�����ͨ�ŵ����ش��ڵľ��
	 */
	EXPORT long WINAPI CreateJWebTop(wstring processPath, wstring configFile);

	/*
	 * �˳�JWebTop����
	 */
	EXPORT void WINAPI ExitJWebTop();

	/*
	* �÷������ڴ���һ�����������
	* jWebTopConfigJSON �����������ϢJSON(JWebTopConfigs.h)
	*
	* return ���ش�������������ڵľ��
	*/
	EXPORT long WINAPI CreateJWebTopBrowser(wstring jWebTopConfigJSON);

	/*
	 * �÷������ڹر�һ�����������
	 * browserHWnd  ��������ھ��
	 */
	EXPORT void WINAPI CloseJWebTopBrowser(long browserHWnd);

	/*
	 * �����ĸ���������ִ��������ű�
	 */
	EXPORT LPTSTR WINAPI JWebTopExecuteJSWait(long browserHWnd, wstring script);
	EXPORT void WINAPI JWebTopExecuteJSNoWait(long browserHWnd, wstring script);
	EXPORT LPTSTR WINAPI JWebTopExecuteJSONWait(long browserHWnd, wstring json);
	EXPORT void WINAPI JWebTopExecuteJSONNoWait(long browserHWnd, wstring json);
}
#endif
