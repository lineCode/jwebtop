//////////////////////////////////////////////////////////////////////////  
/// ����IPCͨ���õķ���˶�
/// 
/// Դ�룺http://git.oschina.net/washheart/fastipc
/// Դ�룺https://github.com/washheart/fastipc
/// ˵����https://github.com/washheart/fastipc/wiki
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-22  
////////////////////////////////////////////////////////////////////////// 
#include "mfc_demo.h"
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "common/util/StrUtil.h"
#include <thread>
#include <string>
#include <sstream>

extern std::wstring serverName;	// ���������
extern fastipc::Server server;  // IPC�����
extern DWORD blockSize;			// һ�δ���ʱ�����ݴ�С

LPTSTR getWindowTitle(){ return L"IPC����֮�����"; }

DWORD startJWebTopProcess(std::wstring clientExe, std::wstring serverName, DWORD blockSize){
	STARTUPINFO sui;
	ZeroMemory(&sui, sizeof(STARTUPINFO));
	sui.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi; // �������������ӽ��̵���Ϣ
	DWORD dwCreationFlags = CREATE_NEW_PROCESS_GROUP;
	// ����������
	std::wstringstream wss; wss << clientExe << L" " << serverName << L" " << blockSize;
	if (CreateProcess(NULL, LPTSTR(wss.str().c_str()), // ��һ�������ÿգ���ִ���ļ��������зŵ�һ���������������
		NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &sui, &pi)){
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return pi.dwThreadId;
	}
	return 0;
}

int startIPCServer(){
	blockSize = 2048;// ָ��һ���������ռ䣬���ں���������̽�������
	serverName = fastipc::genServerName();
	if (server.create(serverName, blockSize) != 0){
		MessageBox(NULL, L"����IPC������ʧ��", L"��Ϣ", 0);
		return 1;
	}

	return 0;
}
int startJWebTop(){
	//std::wstring clientExee = getClientExe();
	//std::wstring getClientExe(){
	//// �õ���������Ŀ¼
	//TCHAR   szPath[1000];
	//GetModuleFileName(NULL, szPath, MAX_PATH);
	//std::wstring path(szPath);
	//path = path.substr(0, path.find_last_of('\\') + 1);
	//path.append(L"TestFastIPCClient.exe");
	//return path;
	//}
	std::wstring clientExee = L"JWebTop.exe";// ��������̵�·��
	if (_waccess(LPTSTR(clientExee.c_str()), 0) != 0){
		serverName.append(L"�ļ��Ҳ�����");
		MessageBox(NULL, LPTSTR(serverName.c_str()), L"��Ϣ", 0);// ����ļ�������
		return 2;
	}
	 else{
		if (startJWebTopProcess(clientExee, serverName, blockSize) == 0){// �����ӽ��̣�ʧ�ܺ󷵻�
			return -1;
		}
	}
	return 0;
}
// ��������
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int iCmdShow){
	blockSize = 2048;// 
	blockSize = 2000;// ָ��һ���������ռ䣬���ں���������̽�������
	if (startIPCServer() != 0)return;// ��һ�����������̵�FastIPCServer

	serverName = fastipc::genServerName();
	std::wstring clientExee = getClientExe();
	if (_waccess(LPTSTR(clientExee.c_str()), 0) != 0){
		serverName.append(L"�ļ��Ҳ�����");
		MessageBox(NULL, LPTSTR(serverName.c_str()), L"��Ϣ", 0);// ����ļ�������
		return 0;
	}
	if (server.create(serverName, blockSize) != 0){
		MessageBox(NULL, L"����IPC������ʧ��", L"��Ϣ", 0);
		return -1;
	}
	else{
		if (startClientProcess(clientExee, serverName, blockSize) == 0){// �����ӽ��̣�ʧ�ܺ󷵻�
			return -1;
		}
	}
	return createWin(hInstance);
}