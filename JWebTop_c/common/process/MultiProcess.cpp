#include "MultiProcess.h"

#include <Windows.h>
#include <string>
#include <strsafe.h>

DWORD createSubProcess(LPTSTR subProcess, LPTSTR szCmdLine){
	STARTUPINFO sui;
	ZeroMemory(&sui, sizeof(STARTUPINFO)); // ��һ���ڴ������㣬�����ZeroMemory, �����ٶ�Ҫ����memset
	sui.cb = sizeof(STARTUPINFO);
	sui.dwFlags = STARTF_USESTDHANDLES;

	PROCESS_INFORMATION pi; // �������������ӽ��̵���Ϣ
	std::wstring cmd = L" "; cmd += szCmdLine;// ׷��һ���ո��������в�����ǰ�棬�����½��̵�szCmdLine��ȡ��������
	if (CreateProcess(subProcess, LPTSTR(cmd.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi))
	{
		CloseHandle(pi.hProcess); // �ӽ��̵Ľ��̾��
		CloseHandle(pi.hThread);  // �ӽ��̵��߳̾����windows�н��̾���һ���̵߳�������ÿ������������һ���߳���ִ��
		return pi.dwThreadId;
	}
	//else{// else�Ĵ�����Ҫ��Ϊ�˷�����VS�����µ��Գ�����ΪVS�������ĳ���Ĭ�ϵĹ���Ŀ¼��$(ProjectDir)�������Ϊ$(SolutionDir)$(Configuration)\����Ҫelse�Ĵ���
	//	TCHAR   szPath[1000];
	//	GetModuleFileName(NULL, szPath, MAX_PATH);
	//	std::wstring path(szPath);
	//	path = path.substr(0, path.find_last_of('\\') + 1);
	//	path += subProcess;
	//	if (CreateProcess(path.c_str(), LPTSTR(cmd.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi))
	//	{
	//		CloseHandle(pi.hProcess); // �ӽ��̵Ľ��̾��
	//		CloseHandle(pi.hThread);  // �ӽ��̵��߳̾����windows�н��̾���һ���̵߳�������ÿ������������һ���߳���ִ��
	//		return pi.dwThreadId;
	//	}

	//}
	return 0;
}


BOOL sendProcessMsg(HWND hWnd, DWORD msgId, LPTSTR msg){
	COPYDATASTRUCT copyData;
	int len = lstrlen(msg);
	if (len > MPMSG_LARGE_LEN)return false;
	if (len > MPMSG_MINI_LEN){
		MPMSG_LARGE mpMsg;
		copyData.dwData = WM_COPYDATA_LARGE;         // ��ʾ�Ǵ�����
		copyData.cbData = sizeof(mpMsg);             // �����͵����ݽṹ�Ĵ�С
		copyData.lpData = &mpMsg;                    // �����͵����ݽṹ
		mpMsg.msgId = msgId;
		StringCbCopy(mpMsg.msg, sizeof(mpMsg.msg), msg);
		return ::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&copyData);
	}
	else{
		MPMSG_MINI mpMsg;
		copyData.dwData = WM_COPYDATA_MINI;          // ��ʾ��С����
		copyData.cbData = sizeof(mpMsg);             // �����͵����ݽṹ�Ĵ�С
		copyData.lpData = &mpMsg;                    // �����͵����ݽṹ
		mpMsg.msgId = msgId;
		if (msgId == WM_COPYDATA_HWND)copyData.dwData = WM_COPYDATA_HWND;
		StringCbCopy(mpMsg.msg, sizeof(mpMsg.msg), msg);
		return ::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&copyData);
	}
}