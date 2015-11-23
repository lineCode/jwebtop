#ifndef _Included_org_jwebtop_dll4csharp
#define _Included_org_jwebtop_dll4csharp
#include <string>
#include <Windows.h>

#define EXPORT __declspec(dllexport)

extern "C" {

	/*
	* ��Ӧorg.fastipc.FastIPCNative���nCreateServer����
	* �÷������ڴ���һ��fastipc�����
	* serverName	���������
	* blockSize		�������ͻ���ͨ��ʱÿ�ε�������
	* result		�������������Ƿ񴴽��ɹ���0=�ɹ�
	* return		������Server��ָ��
	*/
	EXPORT HWND WINAPI nCreateServer(LPTSTR serverName, int blockSize,int& result);

	/*
	* ��Ӧorg.fastipc.FastIPCNative���nStartRead����
	* �÷������ڿ���fastipc����˵Ķ�ȡ�������ɹ���������ڴ˴�
	* nativeServer	ָ������ʵ����ָ��
	* listener		��Ӧ��ȡ������ָ��
	*/
	EXPORT void WINAPI nStartRead(int nativeServer, jobject listener);

	/*
	* ��Ӧorg.fastipc.FastIPCNative���nCloseServer����
	* �÷������ڹر�fastipc����ˣ�ִ�к�nStartRead��������ֹͣ
	* nativeServer	ָ������ʵ����ָ��
	*/
	EXPORT void WINAPI nCloseServer(int nativeServer);

	/*
	* ��Ӧorg.fastipc.FastIPCNative���nCreateClient����
	* �÷������ڴ���һ��fastipc�ͻ���
	* serverName	�ͻ��˶�Ӧ�ķ���˵�����
	* blockSize		�������ͻ���ͨ��ʱÿ�ε�������
	* result		�������������Ƿ񴴽��ɹ���0=�ɹ�
	* return		������Client��ָ��
	*/
	EXPORT HWND WINAPI nCreateClient(LPTSTR serverName, int blockSize,int& result);

	/*
	* ��Ӧorg.fastipc.FastIPCNative���nWriteClient����
	* �÷���������fastipc�ͻ���д�����ݣ�����д��ɺ󣬻ᴥ������˵�����
	* nativeClient	ָ��ͻ���ʵ����ָ��
	* data			Ҫд�������
	* return		�ɹ�д������ݳ���
	*/
	EXPORT int WINAPI nWriteClient(int nativeClient, int userMsgType, long userValue, LPTSTR userShortStr, LPTSTR data);

	/*
	* ��Ӧorg.fastipc.FastIPCNative���nCloseClient����
	* �÷������ڹر�fastipc�ͻ���
	* nativeClient	ָ��ͻ���ʵ����ָ��
	*/
	EXPORT void WINAPI nCloseClient(int nativeClient);

	//typedef char*(CALLBACK *CSharpCallBack)(long browserHWnd, char* s); // ����һ���ص�������csharp��ʵ�ִ˻ص�����
	//EXPORT void WINAPI SetCSharpCallback(CSharpCallBack callBack);      // ���ûص�����
	//EXPORT void WINAPI DelPtrInDLL(void *p);							// ���ڣ�DLL�����ߣ�ɾ��dll�ж����ָ��
	//
	//// ���·�����װjwebtop_dll��Ӧ����
	//EXPORT long  WINAPI nCreateJWebTop(LPTSTR processPath, LPTSTR configFile);
	//EXPORT void  WINAPI nExitJWebTop();
	//EXPORT long  WINAPI nCreateJWebTopBrowser(LPTSTR jWebTopConfigJSON);
	//EXPORT void  WINAPI nCloseJWebTopBrowser(long browserHWnd);
	//EXPORT char* WINAPI nJWebTopExecuteJSWait(long browserHWnd, LPTSTR script);
	//EXPORT void  WINAPI nJWebTopExecuteJSNoWait(long browserHWnd, LPTSTR script);
	//EXPORT char* WINAPI nJWebTopExecuteJSONWait(long browserHWnd, LPTSTR json);
	//EXPORT void  WINAPI nJWebTopExecuteJSONNoWait(long browserHWnd, LPTSTR json);
}
#endif