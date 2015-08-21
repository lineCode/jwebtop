#include "MultiProcess.h"

#include <Windows.h>
#include <string>
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
	bool sendProcessMsg(HWND hWnd, DWORD msgId, LPTSTR msg){
#ifdef JWebTopLog
		writeLog(L"����WM_COPYDATA��Ϣ��");
		writeLog(msg);
		writeLog(L"\r\n");
#endif
		COPYDATASTRUCT copyData;
		int len = lstrlen(msg);
		if (len > MPMSG_LARGE_LEN) return false;         // ̫���ˣ�����65535����������		
		if (len > MPMSG_MINI_LEN){
			MPMSG_LARGE mpMsg;
			copyData.dwData = WM_COPYDATA_LARGE;         // ��ʾ�Ǵ�����
			copyData.cbData = sizeof(mpMsg);             // �����͵����ݽṹ�Ĵ�С
			copyData.lpData = &mpMsg;                    // �����͵����ݽṹ
			mpMsg.msgId = msgId;
			StringCbCopy(mpMsg.msg, sizeof(mpMsg.msg), msg);
			return ::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&copyData) == JWEBTOP_MSG_SUCCESS;
		}
		else{
			MPMSG_MINI mpMsg;
			if (msgId < MPMSG_USER){
				copyData.dwData = msgId;
			}
			else{
				copyData.dwData = WM_COPYDATA_MINI;          // ��ʾ��С����
			}
			copyData.cbData = sizeof(mpMsg);             // �����͵����ݽṹ�Ĵ�С
			copyData.lpData = &mpMsg;                    // �����͵����ݽṹ
			mpMsg.msgId = msgId;
			StringCbCopy(mpMsg.msg, sizeof(mpMsg.msg), msg);
			return ::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&copyData) == JWEBTOP_MSG_SUCCESS;
		}
	}

	// ����WM_COPYDATA��Ϣ
	LRESULT parseProcessMsg(const  LPARAM lParam, DWORD &msgId, wstring &msg){
		COPYDATASTRUCT* copyData = (COPYDATASTRUCT*)lParam;
		if (copyData->dwData == WM_COPYDATA_LARGE){
			MPMSG_LARGE * mpMsg = ((MPMSG_LARGE *)(copyData->lpData));
			msgId = mpMsg->msgId;
			msg = wstring(mpMsg->msg);
		}
		else{
			MPMSG_MINI * mpMsg = ((MPMSG_MINI *)(copyData->lpData));
			msgId = mpMsg->msgId;
			msg = wstring(mpMsg->msg);
		}
#ifdef JWebTopLog
		wstringstream wss;
		wss << L"����WM_COPYDATA��Ϣ��msgId=" << msgId << L"��msg=" << msg << endl;
		writeLog(wss.str());
#endif
		return JWEBTOP_MSG_SUCCESS;
	}
}