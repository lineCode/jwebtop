#include "MultiProcess.h"

#include <Windows.h>
#include <string>
#include <sstream>
#include <strsafe.h>
#include "common/util/StrUtil.h"
#include "common/JWebTopMsg.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;
namespace jw{
	// ����һ���½��̣����ص�����Ϊ���������̵߳�id
	DWORD createSubProcess(LPTSTR subProcess, LPTSTR szCmdLine){
		std::wstring cmd;
		if (subProcess[0] == L'"'){// ���url������˫���ſ�ͷ����ô����Ͳ������˫����
			cmd.append(subProcess);
		}
		else{// ���ӽ�����˫���Ű�����������������������
			cmd.append(L"\"");
			cmd.append(subProcess);
			cmd.append(L"\"");
		}
		cmd += L" "; cmd += szCmdLine;// ׷��һ���ո��������в�����ǰ�棬�����½��̵�szCmdLine��ȡ��������

		STARTUPINFO sui;
		ZeroMemory(&sui, sizeof(STARTUPINFO)); // ��һ���ڴ������㣬�����ZeroMemory, �����ٶ�Ҫ����memset
		sui.cb = sizeof(STARTUPINFO);

		PROCESS_INFORMATION pi; // �������������ӽ��̵���Ϣ
		//DWORD dwCreationFlags = DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP;
		DWORD dwCreationFlags = CREATE_NEW_PROCESS_GROUP;
		if (CreateProcess(NULL, LPTSTR(cmd.c_str()), // ��һ�������ÿգ���ִ���ļ��������зŵ�һ���������������
			NULL, NULL, FALSE,//������̳� 
			dwCreationFlags, //ʹ���������ȼ� 
			NULL, //ʹ�ø����̵Ļ������� 
			NULL, //ָ������Ŀ¼ 
			&sui, &pi))
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

	// ��ָ�����ڷ���WM_COPYDATA��Ϣ��
	// WM_COPYDATA���Կ���̷��ͣ������˷�����ͬ�����������ں�ʱ������յ���Ϣ�Ľ���Ӧ�������̴߳���
	// ��JWebTop�ж��ڽ��յ�WM_COPYDATA��Ϣ���ǿ������̴߳���
	bool sendProcessMsg(const HWND receiverHWnd, const DWORD msgId, const wstring msg, const  long senderHWND, const wstring taskId){
#ifdef JWebTopLog
		wstringstream wss;
		wss<<L"����WM_COPYDATA��Ϣ��msgId=" << msgId << L"��msg=" << msg
			<< ",senderHWnd=" << senderHWND << ",taskId=" << taskId << L"\r\n";
		writeLog(wss.str());
#endif
		COPYDATASTRUCT copyData;
		int len = msg.length();
		if (len > MPMSG_LARGE_LEN) return false;         // ̫���ˣ�����65535����������		
		if (len > MPMSG_MINI_LEN){
			MPMSG_LARGE mpMsg;
			StringCbCopy(mpMsg.msg, sizeof(mpMsg.msg), LPTSTR(msg.c_str()));
			StringCbCopy(mpMsg.taskId, sizeof(mpMsg.taskId), LPTSTR(taskId.c_str()));
			copyData.dwData = msgId + LARGE_MSG_START;
			copyData.cbData = sizeof(mpMsg);             // �����͵����ݽṹ�Ĵ�С
			copyData.lpData = &mpMsg;                    // �����͵����ݽṹ
		}
		else{
			MPMSG_MINI  mpMsg;
			StringCbCopy(mpMsg.msg, sizeof(mpMsg.msg), LPTSTR(msg.c_str()));
			StringCbCopy(mpMsg.taskId, sizeof(mpMsg.taskId), LPTSTR(taskId.c_str()));
			copyData.dwData = msgId;
			copyData.cbData = sizeof(mpMsg);             // �����͵����ݽṹ�Ĵ�С
			copyData.lpData = &mpMsg;                    // �����͵����ݽṹ
		}
		return ::SendMessage(receiverHWnd, WM_COPYDATA, senderHWND, (LPARAM)(LPVOID)&copyData) == JWEBTOP_MSG_SUCCESS;
	}

	// ����WM_COPYDATA��Ϣ
	LRESULT parseProcessMsg(const LPARAM lParam, DWORD &msgId, wstring &msg, std::wstring &taskId){
		COPYDATASTRUCT* copyData = (COPYDATASTRUCT*)lParam;
		if (copyData->dwData >LARGE_MSG_START){
			MPMSG_LARGE * mpMsg = ((MPMSG_LARGE *)(copyData->lpData));
			msg = wstring(mpMsg->msg);
			taskId = wstring(mpMsg->taskId);
			msgId = copyData->dwData - LARGE_MSG_START;
		}
		else{
			MPMSG_MINI * mpMsg = ((MPMSG_MINI *)(copyData->lpData));
			msg = wstring(mpMsg->msg);
			taskId = wstring(mpMsg->taskId);
			msgId = copyData->dwData ;
		}
#ifdef JWebTopLog
		wstringstream wss;
		wss << L"����WM_COPYDATA��Ϣ��msgId=" << msgId << L"��msg=" << msg 
			<< ",senderHWnd=" << senderHWnd << ",taskId=" << taskId << L"\r\n";
		writeLog(wss.str());
#endif
		return JWEBTOP_MSG_SUCCESS;
	}
}