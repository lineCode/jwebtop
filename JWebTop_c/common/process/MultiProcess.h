#ifndef jwebtop_multi_process_H_
#define jwebtop_multi_process_H_

#include <Windows.h>
#include <string>

#define WM_COPYDATA_MINI  1      // ����WM_COPYDATA��Ϣ��dwData����ֵ����ʾ��С����
#define WM_COPYDATA_LARGE 2      // ����WM_COPYDATA��Ϣ��dwData����ֵ����ʾ�Ǵ�����
#define WM_COPYDATA_EXIT  3      // ����WM_COPYDATA��Ϣ��dwData����ֵ����ʾ���˳����ڵ���Ϣѭ��
#define MPMSG_USER        100    // ����MPMSG_MINI��MPMSG_LARGE��msgId����ʼֵ

#define TASKID_LEN        100	 // ����ID�ĳ���

#define LARGE_MSG_START   20000	 // ͨ����ֵ�������Ǵ����ݻ���С���ݣ�������ʱmsgId=dwData-LARGE_MSG_START
#define MPMSG_MINI_LEN    1024   // ����С���ݽṹʱ�ַ����ĳ���
#define MPMSG_LARGE_LEN   65535  // ��������ݽṹʱ�ַ����ĳ���

using namespace std;
namespace jw{
	typedef	struct tagMPMSG_MINI
	{
		TCHAR msg[MPMSG_MINI_LEN];
		TCHAR taskId[TASKID_LEN];
	}MPMSG_MINI;

	typedef	struct tagMPMSG_LARGE
	{
		TCHAR msg[MPMSG_LARGE_LEN];
		TCHAR taskId[TASKID_LEN];
	}MPMSG_LARGE;

	// �����½��̲����ؽ����ж�Ӧ���߳�id������ͨ��PostThreadMessage����֪ͨ����Ϣ��
	DWORD createSubProcess(LPTSTR subProcess, LPTSTR szCmdLine);

	// ���Ϳ������Ϣ
	// receiverHWnd	:������Ϣ�Ĵ��ڵľ��
	// msgId		:����MPMSG_USER����ֵ
	// msg			:�����͵��ַ�����Ϣ
	// senderHWND	:������Ϣ�Ĵ���
	// taskId		:�������ȴ����񣬴�ֵ��Ӧ�ȴ������ID
	bool sendProcessMsg(const HWND receiverHWnd, const DWORD msgId, const wstring msg, const long senderHWND, const wstring taskId);

	LRESULT parseProcessMsg(const  LPARAM lParam, DWORD &msgId, wstring &msg, std::wstring &taskId);
}
#endif